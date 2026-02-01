#version 400 core

in vec3 fPos;
in vec3 fNor;
in vec2 uv;

out vec4 FragColor;

uniform vec3 cameraPosition;

// light (point light)
uniform vec3 lightPosition;
uniform vec3 ambientLightColor;
uniform float ambientLightIntensity;

// material
uniform vec3 baseColor;
uniform float metallic;
uniform float roughness;

uniform sampler2D albedoTex;

void main()
{
    // ---- Normal ----
    vec3 N = normalize(fNor);

    // ---- Light direction ----
    vec3 L = normalize(lightPosition - fPos);

    // ---- View direction ----
    vec3 V = normalize(cameraPosition - fPos);

    // ---- Albedo ----
    vec3 albedo = baseColor;
    if (textureSize(albedoTex, 0).x > 0)
        albedo *= texture(albedoTex, uv).rgb;

    // ---- Ambient ----
    vec3 ambient = albedo * ambientLightColor * ambientLightIntensity;

    // ---- Diffuse (Lambert) ----
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * albedo;

    // ---- Specular (Blinn-Phong) ----
    vec3 H = normalize(L + V);
    float shininess = mix(128.0, 8.0, roughness);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specular = spec * mix(vec3(0.04), albedo, metallic);

    // ---- Attenuation ----
    float distance = length(lightPosition - fPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    vec3 color = (ambient + diffuse + specular) * attenuation;

    FragColor = vec4(color, 1.0);
}
