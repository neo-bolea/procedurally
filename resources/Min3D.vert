#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform Matrices
{
    mat4 uView;
    mat4 uProj;
	mat4 uVP;
};

out vec2 fTexCoord;
out vec3 fNormal;
out vec3 fFragPos;

uniform mat4 uModel;

void main()
{ 
	gl_Position = uVP * uModel * vec4(vPos, 1.f);
    fFragPos = vec3(uModel * vec4(vPos, 1.0));
	fNormal = mat3(transpose(inverse(uModel))) * vNormal; 
	fTexCoord = vUV;
}