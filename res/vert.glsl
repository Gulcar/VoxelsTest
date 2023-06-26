#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aNormal;
layout (location = 2) in uvec3 aColor;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform mat3 uNormalMat;

uniform mat4 uShadowViewProj;

out vec3 FragCoord;
out vec3 Normal;
out vec3 Color;
out vec4 FragShadowCoord;

void main()
{
    vec3 normal;

    switch (aNormal)
    {
    case 0u: normal = vec3(-1, 0, 0); break;
    case 1u: normal = vec3(1, 0, 0); break;
    case 2u: normal = vec3(0, -1, 0); break;
    case 3u: normal = vec3(0, 1, 0); break;
    case 4u: normal = vec3(0, 0, -1); break;
    case 5u: normal = vec3(0, 0, 1); break;
    }

    FragCoord = (uModel * vec4(aPos, 1.0)).xyz;
    Normal = uNormalMat * normal;
    Color = aColor / 255.0f;
    FragShadowCoord = uShadowViewProj * vec4(FragCoord, 1.0);

    gl_Position = uViewProj * uModel * vec4(aPos, 1.0);
}
