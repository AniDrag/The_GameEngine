#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec3 aTan;      // tangent vector
layout (location = 3) in vec3 aBitan;    // bitangent vector
layout (location = 4) in vec2 aUv;

uniform mat4 mvpMatrix; 
uniform mat4 modelMatrix;

out vec3 fPos;          // world position
out vec3 fNor;          // world normal (optional)
out mat3 fTBN;          // tangent → world matrix
out vec2 uv;

void main() {
    vec4 worldPos = modelMatrix * vec4(aPos, 1.0);
    fPos = worldPos.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    vec3 worldNormal = normalize(normalMatrix * aNor);
    vec3 worldTangent = normalize(normalMatrix * aTan);
    vec3 worldBitangent = normalize(normalMatrix * aBitan);

    fTBN = mat3(worldTangent, worldBitangent, worldNormal);
    fNor = worldNormal;   // can be used for fallback or debugging
    uv = aUv;

    gl_Position = mvpMatrix * vec4(aPos, 1.0);
}