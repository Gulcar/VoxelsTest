#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 Color;

const vec3 lightDir = normalize(vec3(0.5f, -1.5f, -0.7f));

void main()
{
    float light = 0.3;
    light += max(dot(-lightDir, Normal), 0.0);

	FragColor = vec4(Color * light, 1.0);
}
