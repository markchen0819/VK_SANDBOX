#version 450

struct Particle
{
	vec4 position;
    vec4 rotation;
    vec4 scale;
	vec4 velocity;
    vec4 color;

    vec4 startPosition;
    vec4 startRotation;
    vec4 startScale;
	vec4 startVelocity;

	float lifeTime;
    float remainingLifetime;
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

} ubo;

//layout (set = 1, binding = 0) uniform ParameterUBO
//{
//    float deltaTime;
//} ubo;
//
//layout(set = 1, binding = 1) readonly buffer ParticleSSBOIn 
//{
//   Particle particlesIn[ ];
//};
layout(set = 1, binding = 2) buffer ParticleSSBOOut 
{
   Particle particles[ ];
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
    float qx = q.x;
    float qy = q.y;
    float qz = q.z;
    float qw = q.w;

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
    // Retrieve transformation data for this particle
    vec3 position = particles[gl_InstanceIndex].position.xyz;
    vec4 rotation = particles[gl_InstanceIndex].rotation;
    vec3 scale = particles[gl_InstanceIndex].scale.xyz;

    // Create a transformation matrix for this particle ( local space to the particle system's local space)
    mat4 modelMatrix = createTransformationMatrix(position, rotation, scale);
    // Transform the vertex position from local to world
    vec4 worldPosition = modelMatrix * vec4(inPosition.xyz, 1.0);
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * push.modelMatrix * worldPosition;

    // Pass other vertex data (color, texture coordinates) to the fragment shader
    fragColor = particles[gl_InstanceIndex].color.xyz;//inColor;
    fragTexCoord = inTexCoord;
}