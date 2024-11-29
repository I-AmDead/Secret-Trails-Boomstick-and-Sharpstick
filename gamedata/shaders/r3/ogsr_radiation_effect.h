// https://www.shadertoy.com/view/tdlyW2

// ��������� ��������� �������
uniform float3 radiation_effect;

// ��������� ��� ������������� �������
struct RadiationEffectConstants
{
    static const float Size = 40.f;
    static const float Power = 2.25f;
    static const float PowerNoise = 1.6f;
    static const float PowerInRadZone = 0.055f;
};

// ��������� ���������� ����� � ��������� [0, 1) �� ������ �������� �����
float random(float n) { return frac(cos(n) * 343.42f); }

// ��������� ���������� ����� � ��������� [1.5, 2)
float random_range() { return 1.5f + frac(sin(dot(float2(37.2f, 42.5f), float2(13.5f, 17.7f))) * 0.5f + 0.5f); }

// ��������� ���������� ����� � �������������� ���� � ��������
float random_IGN(float2 position, float offset = 0.f)
{
    const float3 magic = float3(61.11056f, 5.83715f, 5292.9189f);
    return frac(magic.z * frac(dot(position + offset, magic.xy)));
}

// ��������� ������� ����� �� ��������
float4 generate_green_points(float2 uv, float frequency, float amplitude, float spacing)
{
    // ��������� ���������� uv
    uv.x = uv.x * (screen_res.x * screen_res.w) / (1.2f * random_range()) + timers.x * 0.14f;

    // ���������� � ����� � ����� spacing
    float2 gridUV = round(uv * spacing) / spacing;

    // ���������� ������� ��� �����������
    float smoothingFactor = smoothstep(0.09f, 0.014f, distance(uv, gridUV) * spacing);

    // ��������� �������� ����
    float noiseValue = random_IGN(gridUV, random(timers.x) * 0.1f);

    // ������������ ���� (������� ���� � ������ ������� ����������� � �������� ����)
    return float4(0.f, noiseValue, 0.f, smoothingFactor);
}

float3 rad_effect(float3 color, float2 tc)
{
    float fRadClamp = clamp(radiation_effect.z, 0.f, 1.f) * RadiationEffectConstants::Power;
    float3 radiation = float3(0.f, 0.f, 0.f);

    float factor;
    float2 center = float2(0.5, 0.5);
    float distFromCenter = length(tc.xy - center);
    float fade = smoothstep(0.2, 0.95, distFromCenter);
    float3 green_points;

    float4(green_points, factor) = generate_green_points(tc.xy, 0.85f, 0.9f, RadiationEffectConstants::Size);

    radiation.rgb += (smoothstep(0.9f, 0.99f, (green_points)) * (RadiationEffectConstants::PowerNoise * (fRadClamp * 2) * fade * factor));

    // Output
    return radiation;
}