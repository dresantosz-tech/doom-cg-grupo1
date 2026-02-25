#version 120

uniform sampler2D uTexture;
uniform float uTime;
uniform float threshold;
uniform float error_correction;
uniform vec4 color;

varying vec2 vTexCoord;
varying vec3 vVertex;

void main()
{
    vec4 baseColor = texture2D(uTexture, vTexCoord);

    // Converte os controles do shader original para uma faixa util em UV.
    float t0 = clamp((threshold + 1.0) * 0.5, 0.0, 1.0);
    float t1 = clamp(max(t0 + 0.05, error_correction), 0.0, 1.0);

    float band = step(t0, vTexCoord.y) * step(vTexCoord.y, t1);
    float pulse = 0.55 + 0.45 * sin(uTime * 5.0);

    vec3 tinted = mix(baseColor.rgb, color.rgb, 0.70);
    vec3 finalColor = mix(baseColor.rgb, tinted * (0.8 + 0.2 * pulse), band);

    float alphaBand = clamp((1.0 - vTexCoord.y) * color.a, 0.0, 1.0);
    float alpha = mix(baseColor.a, max(baseColor.a, alphaBand), band);

    gl_FragColor = vec4(finalColor, alpha);
}
