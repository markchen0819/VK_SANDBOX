#version 450

//layout(push_constant) uniform Push
//{
//  mat4 modelMatrix;
//  mat4 normalMatrix;
//} push;


//layout(set = 0, binding = 0) uniform GlobalUniformBufferObject 
//{
//  mat4 modelMatrix;
//  mat4 projectionMatrix;
//  mat4 viewMatrix;
//  mat4 inverseViewMatrix;
//} ubo;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = vec4(fragColor * texture(texSampler, fragTexCoord).rgb, 1.0);
}