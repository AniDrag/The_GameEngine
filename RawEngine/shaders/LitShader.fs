#version 400 core

// Global / scene uniforms
uniform vec3 cameraPosition;          // world space
uniform vec3 lightPosition;           // world space
uniform vec3 ambientLightColor;
uniform float ambientLightIntensity;

// Material uniforms
// base colors
uniform vec3  baseColor;
uniform sampler2D albedoTex;
//metalic
uniform float metallic;
//Rougnes
uniform float roughness;
uniform int   useRoughMap;
uniform sampler2D roughMap;

//normalMap
uniform sampler2D normalMap;          // normal map (tangent space)

// Bloom
uniform float uBloomThreshold;

// Inputs from vertex shader
in vec3 fPos;          // world space position
in vec3 fNor;          // world space normal
in mat3 fTBN;          // tangent→world matrix
in vec2 uv;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{
    // -------- Normal mapping ----------
    // Sample and expand the tangent‑space normal
    vec3 tangentNormal = texture(normalMap, uv).rgb;
    tangentNormal = tangentNormal * 2.0 - 1.0;
    // Transform to world space using TBN matrix
    vec3 N = normalize(fTBN * tangentNormal);

    // -------- Light & view directions (world space) ----------
    vec3 L = normalize(lightPosition - fPos);
    vec3 V = normalize(cameraPosition - fPos);

    // -------- Albedo ----------
    vec3 albedo = baseColor * texture(albedoTex, uv).rgb;

    // -------- Ambient ----------
    vec3 ambient = albedo * ambientLightColor * ambientLightIntensity;

    // -------- Diffuse (Lambert) ----------
    float distance = length(lightPosition - fPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = albedo * NdotL * attenuation;

    // -------- Specular (Phong) ----------
    vec3 R = reflect(-L, N);    // reflection vector (world space)

    // Roughness value
    float rough = (useRoughMap == 1) ? texture(roughMap, uv).r : clamp(roughness, 0.0, 1.0);
    float shininess = mix(128.0, 8.0, rough);   // smooth → high shininess, rough → low shininess

    float specFactor = pow(max(dot(R, V), 0.0), shininess);
    vec3 specColor = mix(vec3(1.0), albedo, metallic);
    vec3 specular = specColor * specFactor * attenuation;

    // -------- Combine ----------
    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0);

    // -------- Bloom brightness extraction ----------
    float brightness = dot(finalColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    float threshold = uBloomThreshold;
    float knee = 0.2;
    float brightnessFactor = 0.0;
    if (brightness > threshold) {
        if (brightness < threshold + knee) {
            brightnessFactor = (brightness - threshold) / knee;
        } else {
            brightnessFactor = 1.0;
        }
    }
    vec3 brightColor = finalColor.rgb * brightnessFactor;
    brightColor = min(brightColor, vec3(10.0));   // optional HDR clamp
    BrightColor = vec4(brightColor, 1.0);
}