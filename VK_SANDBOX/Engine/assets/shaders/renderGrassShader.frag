#version 450

layout (set = 1, binding = 0) uniform ParameterUBO
{
    float deltaTime; float accumulatedTime; float windStrength; float windSpeed;
    float chunkX; float chunkY; float gridSizeX; float gridSizeY;
    float areaSize; float swayStrength; float swayFrequency; int useGlobalTilt;
	float globalTilt; int enableControlPt; float bend; int enableRotationOverride;
	int showWorldNormals; int showLOD; int isHighLOD; int dummy1;
    vec4 windDirection;
    vec4 controlPtA;
    vec4 controlPtB;
    vec4 globalRotation;
} parameterUbo;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragWorldPosition;
layout(location = 4) in vec3 viewPos;

layout(location = 0) out vec4 outColor;

void main() 
{
    // Sample texture
    vec4 texColor = texture(texSampler, fragTexCoord); 

    // Simple lighting
    vec3 lightWorldPosition = vec3(0.0, 1000.0, -1000.0);
    //vec3 lightDir = normalize(lightWorldPosition - fragWorldPosition);
    vec3 lightDir = normalize(lightWorldPosition - vec3(0.0, 0.0, 0.0));
    vec3 ambientColor = vec3(0.2, 0.2, 0.2);
    vec3 diffuseColor = vec3(0.9, 0.9, 0.9);
    vec3 specularColor = vec3(0.3, 0.5, 0.3);
    float shininess = 32.0; 
    vec3 worldNormal = normalize(fragNormal);
    vec3 ambient = ambientColor * texColor.rgb;
    float diff = max(dot(worldNormal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * texColor.rgb;
    vec3 viewDir = normalize(viewPos - fragWorldPosition);
    vec3 reflectDir = reflect(-lightDir, worldNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularColor * spec; 

    diffuse = mix(diffuse, fragColor, 0.05); // color variation
    vec3 finalColor = ambient + diffuse + specular;
    outColor = vec4(finalColor, texColor.a);

    // Debug color variation
    // outColor = vec4(fragColor, texColor.a);

    // Debug world normal
    if (parameterUbo.showWorldNormals == 1)
    {
        outColor = vec4(fragNormal, 1.0); 
    }
    // Debug LOD (Pink)
    if (parameterUbo.showLOD == 1 && parameterUbo.isHighLOD == 0)
    {
        outColor = mix(outColor, vec4(1.0, 0.75, 0.8, 1.0), 0.2); 
    }

    // Debug world position
    //outColor = vec4(normalize(fragWorldPosition), 1);
}

