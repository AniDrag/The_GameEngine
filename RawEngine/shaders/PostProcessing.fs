#version 400 core
out vec4 FragColor;

in vec2 uv;
uniform sampler2D text;

uniform bool uGrayscale;
uniform bool uInvert;

// --- Pixelization ---
uniform bool uPixelize;
uniform float uPixelSize;


void main()
{
    vec2 uvMod = uv;

    if (uPixelize)
    {
        uvMod = floor(uv * uPixelSize) / uPixelSize;
    }
    vec4 color = texture(text, uvMod);

    if (uGrayscale) {
        float g = dot(color.rgb, vec3(0.299, 0.587, 0.114));
        color.rgb = vec3(g);
    }

    if (uInvert) {
        color.rgb = vec3(1.0) - color.rgb;
    }

    FragColor = color;
}
