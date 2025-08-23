#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 uColor;

void main()
{
    FragColor = uColor;
}