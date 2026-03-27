#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBitan;

uniform mat4 modelMatrix;  

out vec3 fPos;
out vec3 fNor;
out mat3 fTBN;
out vec2 uv;

void main()
{
    vec4 worldPos = modelMatrix * vec4(aPos, 1.0);
    fPos = worldPos.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    vec3 worldNormal = normalize(normalMatrix * aNor);
    vec3 worldTangent = normalize(normalMatrix * aTan);
    vec3 worldBitangent = normalize(normalMatrix * aBitan);

    fTBN = mat3(worldTangent, worldBitangent, worldNormal);
    fNor = worldNormal;
    uv = aUv;

    gl_Position = vec4(worldPos.xyz, 1.0);  // or use projection if needed
}