#version 400 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D uSceneTexture;
uniform sampler2D uBloomTexture;
uniform float uBloomIntensity;
uniform bool uGrayscale;
uniform bool uInvert;
uniform bool uPixelize;
uniform float uPixelSize;

void main()
{
    vec2 uvMod = uv;
    if (uPixelize) {
        uvMod = floor(uv * uPixelSize) / uPixelSize;
    }

    vec3 sceneColor = texture(uSceneTexture, uvMod).rgb;
    vec3 bloomColor = texture(uBloomTexture, uvMod).rgb;

    vec3 finalColor = sceneColor + bloomColor * uBloomIntensity;

    if (uGrayscale) {
        float g = dot(finalColor, vec3(0.299, 0.587, 0.114));
        finalColor = vec3(g);
    }
    if (uInvert) {
        finalColor = vec3(1.0) - finalColor;
    }

    FragColor = vec4(finalColor, 1.0);
}