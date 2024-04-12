#version 450

struct FluidParticle
{
	vec4 position;
    vec4 predictPosition;
	vec4 velocity;
    vec4 color;
    vec4 force;
    float pressure;
    float density;
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
   FluidParticle particlesOut[ ];
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

// Helpers
mat4 createTranslationMatrix(vec3 position) 
{
    mat4 matrix = mat4(1.0);
    matrix[3][0] = position.x;
    matrix[3][1] = position.y;
    matrix[3][2] = position.z;
    return matrix;
}

mat4 createTransformationMatrix(vec3 position) 
{
   mat4 translationMatrix = createTranslationMatrix(position);
   float scale = 0.2;
   mat4 precomputedRotScaleMatrix = mat4(
        scale, 0.0, 0.0, 0.0,
        0.0, scale, 0.0, 0.0,
        0.0, 0.0, scale, 0.0,
        0.0, 0.0, 0.0, 1.0);
    return translationMatrix * precomputedRotScaleMatrix;
}
void main() 
{
     // Create a transformation matrix for this particle ( local space to the particle system's local space)
    mat4 modelMatrix = createTransformationMatrix(particlesOut[gl_InstanceIndex].position.xyz);
    // Transform the vertex position from local to world
    vec4 worldPosition = modelMatrix * vec4(inPosition.xyz, 1.0);
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * push.modelMatrix * worldPosition;
    fragColor = particlesOut[gl_InstanceIndex].color.xyz;

}