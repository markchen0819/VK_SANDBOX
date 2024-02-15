#version 450

layout(set = 2, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragWorldPosition;
layout(location = 4) in vec3 viewPos;
layout(location = 0) out vec4 outColor;

void main() 
{
    vec4 texColor = texture(texSampler, fragTexCoord); // Sample texture

     outColor = texColor;

}

