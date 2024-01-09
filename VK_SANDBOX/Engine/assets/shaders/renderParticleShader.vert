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


layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 fragColor;

void main() {

    gl_PointSize = 14.0;
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * push.modelMatrix * vec4(inPosition.xyz, 1.0);
    fragColor = inColor.rgb;
}