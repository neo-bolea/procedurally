#version 330 core
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vUVs;

out vec2 fTexCoords;

uniform Matrices
{
    mat4 uView;
    mat4 uProj;
	mat4 uVP;
};

uniform mat4 uModel;

void main()
{
	gl_Position = uVP * uModel * vec4(vPos.x, vPos.y, 0.f, 1.f);
	fTexCoords = vUVs;
}