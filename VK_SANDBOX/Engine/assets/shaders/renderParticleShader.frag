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

    // Phong lighting
    vec3 lightColor = vec3(1,1,1);
    vec3 lightPos = vec3(100,100,100);
    vec3 viewDir = normalize(viewPos - fragWorldPosition);
    vec3 lightDir = normalize(lightPos - fragWorldPosition.xyz);
    vec3 normal = normalize(fragNormal);


    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    // Specular
    float specularStrength = 0.3;
    int shininess = 128; // Shininess coefficient, higher values for smaller, tighter highlights
    vec3 reflectDir = reflect(-lightDir, fragNormal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

     vec3 lighting = (ambient + diffuse) * fragColor.rgb;


    //outColor = vec4(lighting * texColor.rgb, texColor.a);
    //outColor = vec4(normal * 0.5 + 0.5, 1.0); // Visualize normals
    outColor = vec4(fragColor * texColor.rgb, texColor.a);
}

