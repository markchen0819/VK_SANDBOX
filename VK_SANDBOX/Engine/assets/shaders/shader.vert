#version 450

layout(push_constant) uniform Push
{
  mat4 modelMatrix;
  mat4 normalMatrix;

} push;


layout(set = 0, binding = 0) uniform GlobalUniformBufferObject 
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 inverseView;

} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;
layout(location = 6) in vec3 inBoneIDs;
layout(location = 7) in vec3 inBoneWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() 
{
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * push.modelMatrix * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}

