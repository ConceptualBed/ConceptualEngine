#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse;
uniform float time;
uniform vec4 uColor;

// Funzione di rumore pseudo-casuale
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
    vec2 uv = TexCoords;
    vec4 texColor = texture(texture_diffuse, uv);

    // Effetto di distorsione casuale
    float noise = random(vec2(uv.y, time)) * 0.1;
    uv.x += noise;

    // Linee di scansione
    float scanline = sin(uv.y * 500.0) * 0.05;

    // Glitch di distorsione
    float distortion = sin(uv.x * 2.0 + time) * 0.01;
    uv.y += distortion;
    
    // Ricalcola il colore con la distorsione
    vec4 finalColor = texture(texture_diffuse, uv);
    
    // Aggiungi le linee di scansione
    finalColor -= scanline;

    // Applica il tinting (uColor)
    finalColor *= uColor;

    FragColor = finalColor;
}