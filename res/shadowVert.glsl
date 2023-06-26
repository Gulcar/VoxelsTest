#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aNormal;
layout (location = 2) in uvec3 aColor;

uniform mat4 uModel;
uniform mat4 uViewProj;

void main()
{
    gl_Position = uViewProj * uModel * vec4(aPos, 1.0);
}
