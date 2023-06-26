#version 330 core

out vec4 FragColor;

in vec3 FragCoord;
in vec3 Normal;
in vec3 Color;
in vec4 FragShadowCoord;

uniform vec3 uCameraPos;
uniform sampler2D uShadowMap;

const vec3 lightDir = normalize(vec3(0.5, -1.5, -0.7));
const float shadowBias = 0.0004;

const bool fogEnabled = true;
const vec3 fogColor = vec3(0.471, 0.831, 0.941);

vec3 CalculateFog(vec3 color)
{
    //https://learn.microsoft.com/en-us/windows/win32/direct3d9/fog-formulas
    //float visibility = 1.0 / exp(pow(dist * fogDensity, 2.0));

    float dist = distance(FragCoord, uCameraPos);
    float fog = smoothstep(17.0, 20.0, dist);
    return mix(color, fogColor, fog);
}

float CalculateShadow()
{
    vec3 pos = FragShadowCoord.xyz / FragShadowCoord.w;
    pos = (pos + 1.0) / 2.0;

    if (pos.x < 0.0 || pos.x > 1.0 || pos.y < 0.0 || pos.y > 1.0)
        return 1.0;

    //  b   c
    //    a
    //  d   e

    // texelSize tukaj float ampak ce ne bi bil kvadraten texture bi bil vec2
    //float texelSize = 1.0 / textureSize(uShadowMap, 0).x;
    const float texelSize = 1.0 / 8192.0;

    float a = texture(uShadowMap, pos.xy + vec2(0.0, 0.0)).r;
    float b = texture(uShadowMap, pos.xy + vec2(-texelSize, texelSize)).r;
    float c = texture(uShadowMap, pos.xy + vec2(texelSize, texelSize)).r;
    float d = texture(uShadowMap, pos.xy + vec2(-texelSize, -texelSize)).r;
    float e = texture(uShadowMap, pos.xy + vec2(texelSize, -texelSize)).r;

    float shadow = 1.0;
    float depth = pos.z;

    // nizje vrednosti so blizje kameri

    if (a < depth - shadowBias)
        shadow -= 0.2;
    if (b < depth - shadowBias)
        shadow -= 0.1;
    if (c < depth - shadowBias)
        shadow -= 0.1;
    if (d < depth - shadowBias)
        shadow -= 0.1;
    if (e < depth - shadowBias)
        shadow -= 0.1;

    return shadow;

    /*
    float shadowDepth = texture(uShadowMap, pos.xy).r;
    float depth = pos.z;

    if (shadowDepth < depth - shadowBias)
        return 0.3; // senca

    // ni sence
    return 1.0;
    */
}

void main()
{
    float light = 0.3;
    light += max(dot(-lightDir, Normal), 0.0);

    vec3 color = Color * light;

    if (light != 0.3)
        color *= CalculateShadow();

    if (fogEnabled)
        color = CalculateFog(color);

    FragColor = vec4(color, 1.0);
}
