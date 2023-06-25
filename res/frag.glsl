#version 330 core

out vec4 FragColor;

in vec3 FragCoord;
in vec3 Normal;
in vec3 Color;

uniform vec3 uCameraPos;

const vec3 lightDir = normalize(vec3(0.5, -1.5, -0.7));

const bool fogEnabled = false;
const vec3 fogColor = vec3(0.471, 0.831, 0.941);

vec3 CalculateFog(vec3 color)
{
    //https://learn.microsoft.com/en-us/windows/win32/direct3d9/fog-formulas
    //float visibility = 1.0 / exp(pow(dist * fogDensity, 2.0));

    float dist = distance(FragCoord, uCameraPos);
    float fog = smoothstep(17.0, 20.0, dist);
    return mix(color, fogColor, fog);
}

void main()
{
    float light = 0.3;
    light += max(dot(-lightDir, Normal), 0.0);

    vec3 color = Color * light;

    if (fogEnabled)
        color = CalculateFog(color);

    FragColor = vec4(color, 1.0);
}
