#version 330 core

uniform sampler2D uTex;

out vec4 FragColor;

in vec2 fTexCoords;

void main()
{
    FragColor = texture(uTex, vec2(fTexCoords.x, fTexCoords.y));
}