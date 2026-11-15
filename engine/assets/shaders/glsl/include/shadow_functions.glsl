uniform sampler2D _ShadowMap;
uniform int _HasShadowMap;

float CalcShadow(vec4 lightSpacePosition, vec3 normal, vec3 lightDir)
{
    if (_HasShadowMap == 0)
    {
        return 0.0;
    }

    vec3 projected = lightSpacePosition.xyz / lightSpacePosition.w;
    projected = projected * 0.5 + 0.5;
    if (projected.z > 1.0 ||
        projected.x < 0.0 || projected.x > 1.0 ||
        projected.y < 0.0 || projected.y > 1.0)
    {
        return 0.0;
    }

    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.00005);
    vec2 texelSize = 1.0 / vec2(textureSize(_ShadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(
                _ShadowMap,
                projected.xy + vec2(x, y) * texelSize).r;
            shadow += projected.z - bias > closestDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}
