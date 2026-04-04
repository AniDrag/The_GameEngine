#version 400 core

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 ambientLightColor;
uniform float ambientLightIntensity;

uniform vec3 baseColor;
uniform sampler2D albedoTex;
uniform int useAlbedoMap;

uniform float metallic;
uniform int useMetallicMap;
uniform sampler2D metallicMap;

uniform float roughness;
uniform int useRoughMap;
uniform sampler2D roughMap;

uniform int useNormalMap;
uniform sampler2D normalMap;

uniform float uBloomThreshold;

in vec3 fPos;
in vec3 fNor;
in mat3 fTBN;
in vec2 uv;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{
    vec3 N;
    if (useNormalMap == 1) {
        vec3 tangentNormal = texture(normalMap, uv).rgb;
        tangentNormal = tangentNormal * 2.0 - 1.0;
        N = normalize(fTBN * tangentNormal);
    } else {
        N = normalize(fNor);
    }

    vec3 L = normalize(lightPosition - fPos);
    vec3 V = normalize(cameraPosition - fPos);

    vec3 albedo = baseColor;
    if (useAlbedoMap == 1) {
        albedo *= texture(albedoTex, uv).rgb;
    }

    vec3 ambient = albedo * ambientLightColor * ambientLightIntensity;

    float distanceToLight = length(lightPosition - fPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distanceToLight + 0.032 * distanceToLight * distanceToLight);

    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = albedo * NdotL * attenuation;

    vec3 R = reflect(-L, N);

    float roughValue = roughness;
    if (useRoughMap == 1) {
        roughValue = texture(roughMap, uv).r;
    }
    roughValue = clamp(roughValue, 0.0, 1.0);

    float metalValue = metallic;
    if (useMetallicMap == 1) {
        metalValue = texture(metallicMap, uv).r;
    }
    metalValue = clamp(metalValue, 0.0, 1.0);

    float shininess = mix(128.0, 8.0, roughValue);

    float specFactor = pow(max(dot(R, V), 0.0), shininess);
    vec3 specColor = mix(vec3(1.0), albedo, metalValue);
    vec3 specular = specColor * specFactor * attenuation;

    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0);

    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
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

    vec3 brightColor = finalColor * brightnessFactor;
    brightColor = min(brightColor, vec3(10.0));
    BrightColor = vec4(brightColor, 1.0);
}