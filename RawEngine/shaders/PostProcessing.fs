#version 400 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 brightTextures;


in vec2 uv;
uniform sampler2D text;
//unitfor sampler2D brightTextures;

uniform bool uGrayscale;
uniform bool uInvert;

// --- Pixelization ---
uniform bool uPixelize;
uniform float uPixelSize;

// --- Pixelization ---
uniform bool uBloom;
uniform float uBloomIntensity;

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
   if(uBloom)
{
    vec3 bloom = vec3(0.0);

    float offset[5] = float[](
        0.0, 1.3846153846, 3.2307692308, 5.0769230769, 6.9230769231
    );

    float weight[5] = float[](
        0.2270270270,
        0.3162162162,
        0.0702702703,
        0.0262162162,
        0.0162162162
    );

    vec2 texel = 1.0 / textureSize(brightTextures, 0);

    bloom += texture(brightTextures, uvMod).rgb * weight[0];

    for(int i = 1; i < 5; i++)
    {
        bloom += texture(brightTextures, uvMod + vec2(texel.x * offset[i], 0.0)).rgb * weight[i];
        bloom += texture(brightTextures, uvMod - vec2(texel.x * offset[i], 0.0)).rgb * weight[i];
        bloom += texture(brightTextures, uvMod + vec2(0.0, texel.y * offset[i])).rgb * weight[i];
        bloom += texture(brightTextures, uvMod - vec2(0.0, texel.y * offset[i])).rgb * weight[i];
    }

    color.rgb += bloom * uBloomIntensity;
}

    FragColor = color;
}
