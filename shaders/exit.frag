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

    // Efeito preenchendo todo o QUADRADO do tile (sem mascara circular/eliptica).
    float stripes = 0.88 + 0.12 * sin(vTexCoord.y * 420.0 + uTime * 2.7);
    float waveX = 0.90 + 0.10 * sin(vTexCoord.x * 38.0 - uTime * 1.8);
    float grain = 0.96 + 0.04 * sin((vVertex.x + vVertex.z) * 55.0 + uTime * 8.0);
    float pulse = 0.92 + 0.08 * sin(uTime * 3.2);

    float intensity = stripes * waveX * grain * pulse;
    vec3 tinted = mix(baseColor.rgb, color.rgb, 0.78);
    vec3 finalColor = tinted * (1.02 * intensity);

    gl_FragColor = vec4(finalColor, 1.0);
}
