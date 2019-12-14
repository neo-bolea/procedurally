#version 330 core
out vec4 FragColor;

in vec2 fTexCoords;

uniform sampler2D uTex;
uniform sampler2D uColorRamp;

void main()
{
    FragColor = texture(uColorRamp, vec2(texture(uTex, vec2(fTexCoords.x, 1.f - fTexCoords.y)).r, 0.5f));
}