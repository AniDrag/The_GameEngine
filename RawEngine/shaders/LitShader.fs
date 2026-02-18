#version 400 core

// ------------------------------------------------------------
// Global / scene uniforms (set per-frame by Scene)
// ------------------------------------------------------------
uniform vec3 cameraPosition;          // World-space camera position
uniform vec3 lightPosition;           // World-space light position
uniform vec3 ambientLightColor;       // Ambient light color is a world light properties
uniform float ambientLightIntensity;  // Ambient strength is a world light properties

// ------------------------------------------------------------
// Material uniforms (set per-model by Material)
// ------------------------------------------------------------
uniform vec3  baseColor;              // Base material color
uniform float metallic;               // 0 = not metal, 1 = metal
uniform float roughness;              // 0 = smooth, 1 = rough

uniform sampler2D albedoTex;          // Albedo texture
uniform sampler2D normalTex;          // (unused for now, kept alive)


// Bloom settings
uniform float uBloomThreshold;
// ------------------------------------------------------------
// Inputs from vertex shader
// ------------------------------------------------------------
in vec3 fPos;                         // World-space fragment position
in vec3 fNor;                         // World-space normal
in vec2 uv;                           // Texture coordinates

layout (location = 0) out vec4 FragColor;   // normal HDR scene
layout (location = 1) out vec4 BrightColor; // bright regions

void main()
{
    // Normalize inputs
    vec3 N = normalize(fNor);
    vec3 L = normalize(lightPosition - fPos);     // Light direction
    vec3 V = normalize(cameraPosition - fPos);    // View direction

     // --------------------------------------------------------
    // Albedo
    // --------------------------------------------------------
    vec3 albedo = baseColor * texture(albedoTex, uv).rgb;

    // --------------------------------------------------------
    // Ambient
    // --------------------------------------------------------
    vec3 ambient = albedo * ambientLightColor * ambientLightIntensity;

     // --------------------------------------------------------
    // Diffuse (Lambert)
    // --------------------------------------------------------
    float distance = length(lightPosition - fPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = albedo * NdotL * attenuation;

     // --------------------------------------------------------
    // Specular (Phong)
    // --------------------------------------------------------
    vec3 R = reflect(-L, N);

    // Convert roughness → shininess
    float shininess = mix(128.0, 8.0, clamp(roughness, 0.0, 1.0));

    float specFactor = pow(max(dot(R, V), 0.0), shininess);

    // Metals have colored specular, dielectrics use white
    vec3 specColor = mix(vec3(1.0), albedo, metallic);
    vec3 specular = specColor * specFactor;

    // --------------------------------------------------------
    // Combine lighting
    // --------------------------------------------------------
    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0);

    // --- extract brightness ---
    // Extract brightness with soft knee
    float brightness = dot(finalColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    float threshold = uBloomThreshold; //0.8 default
    float knee = 0.2;        // Soft knee range
    
    float brightnessFactor = 0.0;
    if (brightness > threshold) {
        if (brightness < threshold + knee) {
            // Soft knee region - smooth transition
            brightnessFactor = (brightness - threshold) / knee;
        } else {
            brightnessFactor = 1.0;
        }
    }
    
    vec3 brightColor = finalColor.rgb * brightnessFactor;
    
    // Optional: Clamp bright colors to prevent speckles
    brightColor = min(brightColor, vec3(10.0));  // HDR clamp
    
    BrightColor = vec4(brightColor, 1.0);

    /////////////////////////////////////// OLD for refrence /////
   // Diffuse lighting
   // vec3 normalizedLightDir = normalize(lightPosition);
   // vec3 normalizedNormal = normalize(fNor);
   // vec3 normalizedCameraDir = normalize(cameraPosition - fPos);
   // float diffuseIntensity = max(dot(normalizedNormal, normalizedLightDir), 0.0f);
   // 
   // // Base color (albedo)
   // vec3 albedo = baseColor * texture(albedoTex, uv).rgb;  // Default color + texture
   // 
   // // Ambient component
   // vec3 ambient = ambientLightColor * ambientLightIntensity;
   // 
   // // Diffuse component
   // float lightVertDistance = max(distance(lightPosition, fPos), 0.0f);
   // float attenuation = 1.0 / (1.0 + 0.09 * lightVertDistance + 0.032 * (lightVertDistance * lightVertDistance));
   //
   // vec3 diffuse = diffuseIntensity * albedo * ambientLightColor * attenuation;
   //
   // // Specular component
   // vec3 reflection = normalize(normalizedLightDir - 2.0 * dot(normalizedNormal, normalizedLightDir) * normalizedNormal);
   // float specularIntensity = max(dot(reflection, normalizedCameraDir), 0.0f);
   // 
   //
   //
   // // Combine lighting
   // vec3 finalColor = ambient + diffuse + specularIntensity;
   // 
   // // Clamp and output
   // FragColor = vec4(finalColor, 1.0);

}