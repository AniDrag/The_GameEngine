#version 400 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D uTexture;
uniform bool uHorizontal;  // true for horizontal pass, false for vertical
//uniform float uStrength;   // bloom strength

void main()
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);  // Get texel size
    
    // Gaussian weights (5 samples)
    float weights[5] = float[](
        0.2270270270, 0.1945945946, 0.1216216216,
        0.0540540541, 0.0162162162
    );
    
    vec3 result = texture(uTexture, uv).rgb * weights[0];
    
    if (uHorizontal) {
        // Horizontal blur
        for (int i = 1; i < 5; i++) {
            float offset = float(i) * texelSize.x;
            result += texture(uTexture, uv + vec2( offset, 0.0)).rgb * weights[i];
            result += texture(uTexture, uv + vec2(-offset, 0.0)).rgb * weights[i];
        }
    } else {
        // Vertical blur
        for (int i = 1; i < 5; i++) {
            float offset = float(i) * texelSize.y;
            result += texture(uTexture, uv + vec2(0.0,  offset)).rgb * weights[i];
            result += texture(uTexture, uv + vec2(0.0, -offset)).rgb * weights[i];
        }
    }
    
    // Apply bloom strength
    //result *= uStrength;
    
    FragColor = vec4(result, 1.0);
}