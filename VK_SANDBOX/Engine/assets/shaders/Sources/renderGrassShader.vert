#version 450

struct GrassBlade
{
	vec4 position;
    vec4 rotation;
    vec4 scale;
    vec4 color;
    float windStrength;
    float tilt;
    uint perBladeHash;
};

// Push constants
layout(push_constant) uniform Push
{
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

// Descriptor
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject 
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 inverseView;
    vec4 cameraPosition;
} ubo;

layout (set = 1, binding = 0) uniform ParameterUBO
{
    float deltaTime; float accumulatedTime; float windStrength; float windSpeed;
    float chunkX; float chunkY; float gridSizeX; float gridSizeY;
    float areaSize; float swayStrength; float swayFrequency; int useGlobalTilt;
	float globalTilt; int enableControlPt; float bend; int enableRotationOverride;
	int showWorldNormals; int showLOD; int isHighLOD; int dummy;
    vec4 windDirection;
    vec4 controlPtA;
    vec4 controlPtB;
    vec4 globalRotation;
    vec4 spherePosition;
} parameterUbo;


//layout(set = 1, binding = 1) readonly buffer GrassSSBOIn 
//{
//   GrassBlade grassBladesIn[ ];
//};
layout(set = 1, binding = 2) buffer GrassSSBOOut 
{
   GrassBlade grassBladesOut[ ];
};

// Vertex Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;
layout(location = 6) in ivec4 inBoneIDs;
layout(location = 7) in vec4 inBoneWeights;

// Output
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragWorldPosition;
layout(location = 4) out vec3 viewPos;

// Helpers
mat4 createTranslationMatrix(vec3 position) 
{
    mat4 matrix = mat4(1.0);
    matrix[3][0] = position.x;
    matrix[3][1] = position.y;
    matrix[3][2] = position.z;
    return matrix;
}
mat4 createScaleMatrix(vec3 scale) 
{
    mat4 matrix = mat4(1.0);
    matrix[0][0] = scale.x;
    matrix[1][1] = scale.y;
    matrix[2][2] = scale.z;
    return matrix;
}
mat4 quatToMat4(vec4 q) 
{
    // Converts a quaternion to a rotation matrix

    // (1,0,0,0) format to (0,0,0,1) format
    float qx = q.w;
    float qy = q.x;
    float qz = q.y;
    float qw = q.z;

    mat4 matrix = mat4(1.0); // Identity matrix
    matrix[0][0] = 1.0 - 2.0 * qy * qy - 2.0 * qz * qz;
    matrix[0][1] = 2.0 * qx * qy - 2.0 * qz * qw;
    matrix[0][2] = 2.0 * qx * qz + 2.0 * qy * qw;

    matrix[1][0] = 2.0 * qx * qy + 2.0 * qz * qw;
    matrix[1][1] = 1.0 - 2.0 * qx * qx - 2.0 * qz * qz;
    matrix[1][2] = 2.0 * qy * qz - 2.0 * qx * qw;

    matrix[2][0] = 2.0 * qx * qz - 2.0 * qy * qw;
    matrix[2][1] = 2.0 * qy * qz + 2.0 * qx * qw;
    matrix[2][2] = 1.0 - 2.0 * qx * qx - 2.0 * qy * qy;

    return matrix;
}
mat4 createTransformationMatrix(vec3 position, vec4 rotation, vec3 scale) 
{
    mat4 translationMatrix = createTranslationMatrix(position);
    mat4 rotationMatrix = quatToMat4(rotation);
    mat4 scaleMatrix = createScaleMatrix(scale);
    return translationMatrix * rotationMatrix * scaleMatrix;
}
vec3 calculateDerivativeOnCubicBezier(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3) 
{
    //  P'(t) = 3(1 - t)^2 (p1 - p0) + 6(1 - t)t (p2 - p1) + 3t^2 (p3 - p2)
    vec3 p0Prime = 3.0 * (p1 - p0);
    vec3 p1Prime = 3.0 * (p2 - p1);
    vec3 p2Prime = 3.0 * (p3 - p2);
    float u = 1.0 - t;
    vec3 tangent = u * u * p0Prime + 2.0 * u * t * p1Prime + t * t * p2Prime;
    return tangent;
}
vec3 calculatePointOnCubicBezier(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
    // P(t) = (1 - t)^3 p0 + 3(1 - t)^2t p1 + 3(1 - t)t^2 p2 + t^3 p3
    float u = 1.0 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    vec3 p = uuu * p0;
    p += 3.0 * uu * t * p1;
    p += 3.0 * u * tt * p2;
    p += ttt * p3;
    return p;
}
float calculateTforBezier(float y, float startY, float endY)
{
    float t = (y - startY) / (endY - startY);
    t = clamp(t, 0.0, 1.0);
    return t;
}
mat3 calculateTiltRotationMatrix(vec3 axis, float angleInDegree)
{
    // Angle axis rotation in matrix form
    float angleRad = radians(angleInDegree);
    float c = cos(angleRad);
    float s = sin(angleRad);
    float t = 1.0 - c;
    vec3 axisNormalized = normalize(axis);
    float x = axisNormalized.x, y = axisNormalized.y, z = axisNormalized.z;

    mat3 tiltRotationMatrix = mat3
    (
        t*x*x + c,    t*x*y - s*z,  t*x*z + s*y,
        t*x*y + s*z,  t*y*y + c,    t*y*z - s*x,
        t*x*z - s*y,  t*y*z + s*x,  t*z*z + c
    );

    return tiltRotationMatrix;
}

// Interaction 
// (Should change to better solution someday -> RED: Interactive Wind and Vegetation in 'God of War')
// https://youtu.be/MKX45_riWQA?si=cgHpRztP4WC9o4AX
vec3 adjustPositionBasedOnSphereCollsion(vec3 vertPos, vec3 sphereCenter, float radius)
{
    float distanceToSphere = length(vertPos - sphereCenter);
    if (distanceToSphere < radius) 
    {
        vec3 direction = normalize(vertPos - sphereCenter);
        vertPos = sphereCenter + direction * radius;
    }
    return vertPos;
}

// REF: Procedural Grass in 'Ghost of Tsushima'
// https://www.youtube.com/watch?v=Ibe1JBF5i5Y
void main() 
{
    /////////////////////    Model Space    /////////////////////
    //// Cubic Bezier Curve for grassBlade curvature
    vec3 facingVec = vec3(-1.0, 0.0, 0.0); // grass blade model facing
    float tilt = grassBladesOut[gl_InstanceIndex].tilt;
    if(parameterUbo.useGlobalTilt == 1)
    {
        tilt = parameterUbo.globalTilt;
    }
    mat3 tiltMat = calculateTiltRotationMatrix(vec3(0,0,1), tilt); 
    vec3 p0 = vec3(0, 0, 0);
    vec3 p3 = vec3(0, 1.062001, 0);
    p3 =  tiltMat * (p3 - p0) + p0;
    vec3 controlPointDirection = cross(vec3(0,0,1), (p3-p0));
    float bend =  parameterUbo.bend;
    vec3 p1 = p0 + (p3 - p0) * 0.5f + controlPointDirection * bend;
    vec3 p2 = p1; 
    if(parameterUbo.enableControlPt == 1)
    {
        p1 = parameterUbo.controlPtA.rgb;
        p2 = parameterUbo.controlPtB.rgb; 
    }
    float t = calculateTforBezier(inPosition.y, 0.0, 1.062001);
    vec3 curvedPosition = calculatePointOnCubicBezier(t, p0, p1, p2, p3);
    curvedPosition = vec3(curvedPosition.x, curvedPosition.y, inPosition.z);
    //// Creat normals for lighting
    vec3 curvetangent = calculateDerivativeOnCubicBezier(t, p0, p1, p2, p3);
    curvetangent = normalize(curvetangent);
    vec3 up = vec3(0.0, 1.0, 0.0); 
    vec3 orthogonalVec = normalize(cross(curvetangent, up));
    vec3 curveNormal = normalize(cross(orthogonalVec, curvetangent)); 
    curveNormal.x += inPosition.z; // point slightly outwards

    /////////////////////    Particle Local Space    /////////////////////
    vec3 position = grassBladesOut[gl_InstanceIndex].position.xyz;
    vec4 rotation = grassBladesOut[gl_InstanceIndex].rotation;
    vec3 scale = grassBladesOut[gl_InstanceIndex].scale.xyz;
    if(parameterUbo.enableRotationOverride == 1)
    {
        rotation = parameterUbo.globalRotation;
    }
    mat4 modelMatrix = createTransformationMatrix(position, rotation, scale);


    /////////////////////    World Space    /////////////////////
    vec4 worldPosition = push.modelMatrix * modelMatrix * vec4(curvedPosition, 1.0);
    vec3 modelSpaceNormal = normalize(curveNormal); 
    vec3 particleNormal = normalize(mat3(transpose(inverse(modelMatrix))) * modelSpaceNormal);
    vec3 worldNormal =  normalize(mat3(transpose(inverse( push.modelMatrix))) * particleNormal);
    // const float forceInfluenceByHeightForWind = curvedPosition.y;
    // const float forceInfluenceByHeightForSway = inPosition.y;

    // Interation with sphere
    vec3 fixedPosition = adjustPositionBasedOnSphereCollsion(worldPosition.xyz, parameterUbo.spherePosition.xyz, 5.00);
    worldPosition = vec4(fixedPosition,worldPosition.w);

    const float forceInfluenceByHeightForWind = worldPosition.y;
    const float forceInfluenceByHeightForSway = worldPosition.y;

    //// Wind (XZ)
    float windMultiplier = parameterUbo.windStrength; // user defined
    float windStrength = grassBladesOut[gl_InstanceIndex].windStrength; // from noise texture
    float windIntensity = windStrength * forceInfluenceByHeightForWind * windMultiplier;
    worldPosition += parameterUbo.windDirection * windIntensity;     // normalize(parameterUbo.windDirection) do this in CPU to save performance

    //// Swaying (Y)
    float swayMultiplier = parameterUbo.swayStrength * forceInfluenceByHeightForSway;
    float swayFrequency = parameterUbo.swayFrequency;
    float phaseOffset = float(grassBladesOut[gl_InstanceIndex].perBladeHash);
    float swayIntensity = sin(phaseOffset + parameterUbo.accumulatedTime * swayFrequency) * forceInfluenceByHeightForSway * swayMultiplier;
    worldPosition += vec4(0, swayIntensity, 0, 0);

//    // Interation with sphere (2nd fix to make sure wind doesn't move inwards)
//    fixedPosition = adjustPositionBasedOnSphereCollsion(worldPosition.xyz, vec3(0,5,0), 5.02);
//    worldPosition = vec4(fixedPosition,worldPosition.w);

    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * worldPosition;

    /////////////////////    Pass info to fragment shader    /////////////////////
    fragColor = grassBladesOut[gl_InstanceIndex].color.xyz;
    fragTexCoord = inTexCoord;
    fragWorldPosition = worldPosition.xyz;
    fragNormal = worldNormal;
    viewPos = ubo.cameraPosition.xyz;
}