#version 450

struct GrassBlade
{
	vec4 position;
    vec4 rotation;
    vec4 scale;
    vec4 color;
    float windStrength;
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
    float deltaTime; 
    float accumulatedTime;
    float dummy1;
	float dummy2;
	vec4 windDirection;
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

void main() 
{
    // Curving grass blade (TO:DO replace with bezier)
    float curveIntensity = exp(inPosition.y) - 1.0; 
    vec3 curvedPosition = inPosition +  vec3(-0.2,0,0.0) * curveIntensity; 

    // local space to particle local space
    vec3 position = grassBladesOut[gl_InstanceIndex].position.xyz;
    vec4 rotation = grassBladesOut[gl_InstanceIndex].rotation;
    vec3 scale = grassBladesOut[gl_InstanceIndex].scale.xyz;
    mat4 modelMatrix = createTransformationMatrix(position, rotation, scale);

    // partical local space to world space
    vec4 worldPosition = push.modelMatrix * modelMatrix * vec4(curvedPosition, 1.0);
    const float forceInfluenceByHeight =  inPosition.y;

    // Wind (XZ)
    float windMultiplier = 2.0;
    float windStrength = grassBladesOut[gl_InstanceIndex].windStrength;
    float windIntensity = windStrength * forceInfluenceByHeight * windMultiplier;
    worldPosition += normalize(parameterUbo.windDirection) * windIntensity;
    
    // Swaying (Y)
    float swayMultiplier = 0.1 * windIntensity;
    float swayFrequency = 2.0;
    float swayIntensity = sin(parameterUbo.accumulatedTime * swayFrequency) * forceInfluenceByHeight * swayMultiplier;
    worldPosition += vec4(0,1,0,0) * swayIntensity;


    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * worldPosition;



    // Pass other vertex data (color, texture coordinates) to the fragment shader
    fragColor = grassBladesOut[gl_InstanceIndex].color.xyz;//inColor;
    fragTexCoord = inTexCoord;
    // Transform the normal from local to world 
    // normals need to be transformed by the inverse transpose of the model matrix
    // to maintain their orientation properly after scaling
    fragWorldPosition = worldPosition.xyz;
    vec3 worldNormal = mat3(transpose(inverse(modelMatrix))) * inNormal;
    fragNormal = worldNormal;
    viewPos = ubo.cameraPosition.xyz;

}