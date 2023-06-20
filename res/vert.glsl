#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform mat3 uNormalMat;

out vec3 Normal;
out vec3 Color;

void main()
{
    Normal = uNormalMat * aNormal;
    Color = aColor;

	gl_Position = uViewProj * uModel * vec4(aPos, 1.0);
}
