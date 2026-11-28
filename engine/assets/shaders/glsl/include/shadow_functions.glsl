uniform sampler2D _ShadowMap;
uniform int _HasShadowMap;
uniform int _ShadowLightIndex;
uniform samplerCube _PointShadowMap;
uniform int _HasPointShadowMap;
uniform int _PointShadowIndex;
uniform vec3 _PointShadowPosition;
uniform float _PointShadowFarPlane;

float CalcPointShadow(vec3 fragPos, vec3 normal, vec3 lightDir)
{
    if (_HasPointShadowMap == 0)
    {
        return 0.0;
    }

    vec3 offset = fragPos - _PointShadowPosition;
    float currentDepth = length(offset);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec3 direction = normalize(offset);
    vec3 tangent = normalize(abs(direction.z) < 0.999
        ? cross(direction, vec3(0.0, 0.0, 1.0))
        : cross(direction, vec3(0.0, 1.0, 0.0)));
    vec3 bitangent = cross(direction, tangent);
    const float diskRadius = 0.012;
    const vec3 offsets[8] = vec3[8](
        vec3(1.0, 1.0, 1.0),
        vec3(-1.0, 1.0, 1.0),
        vec3(1.0, -1.0, 1.0),
        vec3(-1.0, -1.0, 1.0),
        vec3(1.0, 1.0, -1.0),
        vec3(-1.0, 1.0, -1.0),
        vec3(1.0, -1.0, -1.0),
        vec3(-1.0, -1.0, -1.0));
    for (int i = 0; i < 8; ++i)
    {
        vec3 sampleDir = normalize(direction + diskRadius * (
            tangent * offsets[i].x + bitangent * offsets[i].y + direction * offsets[i].z));
        float closestDepth = texture(_PointShadowMap, sampleDir).r * _PointShadowFarPlane;
        shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
    return shadow / 8.0;
}

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
