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


const int MAX_BONE_INFLUENCE = 4;
layout(set = 2, binding = 0) uniform SkeletalUniformBufferObject
{
    mat4 finalBonesMatrices[100];
    bool hasAnimation;   

} skeletalubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;
layout(location = 6) in ivec4 inBoneIDs;
layout(location = 7) in vec4 inBoneWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() 
{
    if(skeletalubo.hasAnimation)
    {
         vec4 totalPosition = vec4(0.0f);
         for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
         {
            if(inBoneIDs[i] == -1) 
                continue;
            if(inBoneIDs[i] >= 100) 
            {
                totalPosition = vec4(inPosition,1.0f);
                break;
            }
            vec4 localPosition = skeletalubo.finalBonesMatrices[inBoneIDs[i]] * vec4(inPosition,1.0f);
            totalPosition += localPosition * inBoneWeights[i];
            vec3 localNormal = mat3(skeletalubo.finalBonesMatrices[inBoneIDs[i]]) * inNormal;
        }
        mat4 viewModel =  ubo.viewMatrix * push.modelMatrix;
        gl_Position = ubo.projectionMatrix * viewModel * totalPosition;
        fragColor = inColor;
        fragTexCoord = inTexCoord;
        return;
    }
    else
    {
        gl_Position = ubo.projectionMatrix * ubo.viewMatrix * push.modelMatrix * vec4(inPosition, 1.0);
        fragColor = inColor;
        fragTexCoord = inTexCoord;
    }

}

