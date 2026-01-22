#version 400 core

uniform vec3 cameraPosition;          // Camera Position
uniform vec3 lightPosition;           // Light direction (normalized)
uniform vec3 ambientLightColor;       // Ambient light color
uniform float ambientLightIntensity;  // Ambient light strength

uniform vec3 baseColor;
uiniform float metallic;
uniform float roughness;
uniform sample2D albedoTex;
uniform sample2D normalTex;


out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 uv;


void main()
{
   // Diffuse lighting
    vec3 normalizedLightDir = normalize(lightPosition);
    vec3 normalizedNormal = normalize(fNor);
    vec3 normalizedCameraDir = normalize(cameraPosition);
    float diffuseIntensity = max(dot(normalizedNormal, normalizedLightDir), 0.0f);
    
    // Base color (albedo)
    vec3 albedo = vec3(0.1f, 0.2f, 0.8f);  // Default blue-ish color
    
    // Ambient component
    vec3 ambient = ambientLightColor * ambientLightIntensity;
    
    // Diffuse component
    float lightVertDistance = max(distance(lightPosition, fPos), 0.0f);
    float attenuation = 1.0 / (1.0 + 0.09 * lightVertDistance + 0.032 * (lightVertDistance * lightVertDistance));

    vec3 diffuse = diffuseIntensity * albedo * ambientLightColor * attenuation;

    // Specular component
    vec3 reflection = normalize(normalizedLightDir - 2.0 * dot(normalizedNormal, normalizedLightDir) * normalizedNormal);
    float specularIntensity = max(dot(reflection, normalizedCameraDir), 0.0f);
    


    // Combine lighting
    vec3 finalColor = ambient + diffuse + specularIntensity;
    
    // Clamp and output
    FragColor = vec4(finalColor, 1.0);

}