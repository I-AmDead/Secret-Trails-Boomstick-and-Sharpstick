#include "models_watch_effects_common.h"

float3 watch_seconds(float2 uv)
{
    // ��������� ������� ���������� ���������
    uv.x = resize(uv.x, screen_res.x / screen_res.y, 0.0);

    // ��������� ���������� ��������� � �������������� �� � ���������� ������
    float2 p = uv;
    p -= 0.5;
    p.x *= screen_res.x * screen_res.w;

    // ��������� �������� ������� ����
    float time = game_time.z;

    // ������ ���� �������� �� ������ �������
    float angle = (time * 1.0 / 60.0) * 2.0 * PI;

    // �������� ������� �������� 2x2
    float2x2 rot = float2x2(cos(angle), sin(angle), -sin(angle), cos(angle));

    // ���������� �������� � ���������� ����������� � ��������������� �� 0.73
    p = mul(rot, p) * 0.73;

    // ��������� �������� ����� � ������
    float3 color_base = float3(0.0, 0.0, 0.0);

    // ������ ���������� �� ������ ������
    float L = length(p);

    // ������������� ���������� ��� ���������
    float f = 0.;

    // ������������� smoothstep ��� �������� �������� �������� � ���������
    f = smoothstep(L - 0.005, L, 0.35);
    f -= smoothstep(L, L + 0.005, 0.33);

    // ��������� ������� ��� �������� � ��������� ��������� �����
    float t = fmod(time, TPI) - PI;
    float t1 = -PI;
    float t2 = PI;
    float t3 = PI - 6.1;

    // ������ ���� �� ������ ���������� ���������
    float a = atan2(p.x, p.y);

    // ��������� �� ������ ���� � �������
    float f_final = f * step(a, t3);
    f = f * step(a, t2);

    // ������������� �������� ������������ ��� ���������� ������ �� ������ ��������� ��������
    float3 col2 = lerp(color_base, float3(0.21, 0.21, 0.21), f);
    float3 col3 = lerp(color_base, float3(cos(timers.x), cos(timers.x + TPI / 3.0), cos(timers.x + 2.0 * TPI / 3.0)), f_final);

    // ��������� �������� ����� ��� �������� ������
    float4 fragColor1 = float4(col2, 0.0);
    float4 fragColor2 = float4(col3, 0.0);

    // ���������� �������������� ������������ �� ������ ������������� �����
    if (fragColor1.r > 0.2 || fragColor1.g > 0.2 || fragColor1.b > 0.2)
        fragColor1.a = 0.5;

    if (fragColor2.r > 0.3 || fragColor2.g > 0.3 || fragColor2.b > 0.3)
        fragColor2.a = 0.8;

    float4 final = fragColor1 + fragColor2;

    return final.rgb;
}

// https://www.shadertoy.com/view/styBzt
float3 watch_loading(float2 uv)
{
    uv -= 0.5;

    float3 color = float3(0.0, 0.0, 0.0);

    float d = abs(length(uv) - 0.25) - 0.025;
    float3 rainbow = hsv(float3(hue(uv), 1.0, 1.0));

    float t = fmod((timerX10) * 0.9, 3.0);

    float phaseOffset = 3.0 * TPI / 8.0;
    float a = atan2(uv.y, uv.x);
    a /= TPI;
    a += 1.0;
    a = frac(a);
    a = clamp(a, 0.0, 1.0);
    a += 1.0 / 8.0;
    float aRepeat = floor(a * 4.0);
    uv = rotate(uv, aRepeat * TPI / 4.0);

    float h = hue(uv);

    float r = 0.25 * (easeSpring(0.0, 1.0, t) - easeSpring(2.0, 3.0, t));
    float radius = 0.045;
    float tSweep = easeInOutExpo(stepLinear(1.0, 2.0, t));
    float aDot = tSweep * TPI / 4.0 - TPI / 8.0;
    float innerD = 1.0;
    for (float i = 0.0; i <= 3.0; i++)
    {
        float2 pDot2 = rotate(uv, aDot + TPI / 4.0 * i);
        float arcLength = TPI / 16.0 * tSweep;
        float arc = sdArc(pDot2, float2(sin(arcLength), cos(arcLength)), r, radius);
        innerD = min(innerD, arc);
    }

    float hueMix = 1.0 - abs((r / 0.25) - 1.0);
    float dCut = smoothstep(screen_res.z, -screen_res.z, innerD);
    color = lerp(color, float3(1.0, 1.0, 1.0), dCut);
    color = lerp(color, color * rainbow, hueMix);

    return color;
}

// https://www.shadertoy.com/view/3dVXzh
float3 glitch_cube(float2 uv)
{
    // ��������� ������� ���������� ���������
    uv.x = resize(uv.x, screen_res.x / screen_res.y, 0.0);

    // ��������� ���������� ��������� � �������������� �� � ���������� ������
    float2 p = uv;
    p -= 0.5;
    p.x *= screen_res.x * screen_res.w;
    p *= 2.0;

    float3 col = float3(0.0, 0.0, 0.0);

    float3 ro = float3(0.0, -0.2 * ft, 4.0 - 2.0 * ft);
    if (hash(frac(it * 432.543)) < 0.5)
    {
        ro.x += hash(it * 12.9898) * 2.0 - 1.0;
        ro.y += hash(it * 78.233) * 2.0 - 1.0;
    }

    float3 ta = float3(0.0, 0.0, 0.0);

    float cr = 0.0;
    float3 cz = normalize(ta - ro);
    float3 cx = normalize(cross(cz, float3(sin(cr), cos(cr), 0.0)));
    float3 cy = normalize(cross(cx, cz));
    float3 rd = normalize(mul(float3x3(cx, cy, cz), float3(p, 2.0)));

    col = render(ro, rd, rd.xy);
    col *= smoothstep(0.0, 0.2, 1.0 - abs(ft * 2.0 - 1.0));

    return col;
}

// https://www.shadertoy.com/view/Xd3cR8
float3 pda_loading(float2 uv)
{
    // ��������� ������� ���������� ���������
    uv.x = resize(uv.x, 0.6, 0.0); // ��� ��������������� ��� ��� ui
    uv -= 0.5;
    uv.x = -uv.x; 

    float geo = ring(uv, float2(0.0, 0.0), RADIUS - THICCNESS, RADIUS);

    float rot = timers.z * SPEED; // �� GL -timers.z

    uv = mul(uv, float2x2(cos(rot), sin(rot), -sin(rot), cos(rot)));

    float a = atan2(uv.x, uv.y) * PI * 0.05 + 0.5;

    a = max(a, circle(uv, float2(0.0, -RADIUS + THICCNESS / 2.0), THICCNESS / 2.0));

    return a * geo;
}

float3 dosimeter(float2 uva)
{
    float2 uv = uva;

    //uv -= 0.5;
    //uv.y = -uv.y;

    uv *= 280.0;
    float deg = radiansToDegrees(atan2(uv.y, uv.x));
    
    float4 rgba = float4(0.0, 0.0, 0.0, 1.0);
    
    if(uv.y > 0.0 && length(uv) < 190.0 && length(uv) > 170.0)
    {
        rgba = float4(1.0, 1.0, 1.0, 1.0);
        if(deg < 120.0)
            rgba = float4(1.0, 1.0, 0.0, 1.0);
            
        if(deg < 60.0)
            rgba = float4(1.0, 0.0, 0.0, 1.0);
            
        if(deg > 120.0)
            rgba = float4(0.0, 1.0, 0.0, 1.0);
            
        if(fmod(deg, 12.0) < 2.0 && length(uv) > 175.0)
            rgba = float4(0.0, 0.0, 0.0, 1.0);
        else if(fmod(deg, 12.0) < 2.0 && length(uv) <= 175.0)
            rgba = float4(0.0, 0.0, 0.0, 1.0);
    }
    
    float rad_level = radiation_effect.z;
    rad_level = clamp(rad_level, 0.0, 1.0);
    uv = mul(uv, rotate(-abs(sin(1.4) * rad_level) * PI));
    
    if(uv.x < 0.0 && abs((uv.x + 180.0) / 50.0) > abs(uv.y) && uv.x > -160.0 || length(uv) < 6.0)
        rgba = float4(0.5, 0.4, 0.1, 1.0);

    return rgba.rgb;
}

float3 NixieTime(float2 uv) 
{
    uv.x = resize(uv.x, screen_res.x / screen_res.y, 0.0);

    float2 uva = uv;
    //uva.y -= 0.2;

    uv -= 0.5;
    uv *= 1.1;
    uv.x *= screen_res.x * screen_res.w;
    uv.y = -uv.y;

    float hour = game_time.x;
    float minute = game_time.y;
    float seconds = game_time.z;
    float miliseconds = game_time.w;
    float radiation = watch_actor_params.z;
    
	float nsize = numberLength(9999.0);
    float2 digit_spacing = mul(float2(1.1, 1.6), 1.0 / 6.0);
	float2 pos = -digit_spacing * float2(nsize, 1.0) / 2.0;

    float2 basepos = pos;
	float dist = 1.0;

	float3 color = float3(0.0, 0.0, 0.0);

    if (watch_actor_params.w == 1)
    {
        pos.x = basepos.x + 0.16;
	    dist = min(dist, dfNumber(pos, hour, uv));
    
        pos.x = basepos.x + 0.39;
	    dist = min(dist, dfColon(pos, uv));
    
        pos.x = basepos.x + 0.60;
	    dist = min(dist, dfNumber(pos, minute, uv));
    }

    if (watch_actor_params.w == 4)
    {
        uv *= 1.5;

        pos.x = basepos.x - 0.15;
	    dist = min(dist, dfNumber(pos, minute, uv));
    
        pos.x = basepos.x + 0.1;
	    dist = min(dist, dfColon(pos, uv));
    
        pos.x = basepos.x + 0.35;
	    dist = min(dist, dfNumber(pos, seconds, uv));

        pos.x = basepos.x + 1.75;
        pos.y = basepos.y - 0.75;
        dist = min(dist, dfCircle(pos, 0.04, uv));

        pos.x = basepos.x + 0.8;
        pos.y = basepos.y;
	    dist = min(dist, dfNumber(pos, miliseconds, uv));
    }

    if (watch_actor_params.w == 3)
    {
        uv *= 1.3;
        pos = basepos;
        pos.x += 0.15;
        pos.y -= 0.3;
	    dist = min(dist, dfNumber2(pos, radiation, uv));

        color += dosimeter(uv);
    }
	
	float shade = 0.004 / (dist);
	
	color += watch_color_params.rgb * shade;
#if GLOWPULSE
	color += (watch_color_params.rgb * 0.5) * shade * noiseNixie((uv + float2(timers.y * 0.2, 0.0)) * 2.5 + float2(0.5, 0.0));
#endif

//	color += (game_time.x > 21 || game_time.x < 4 ? float3(0.0, 0.9, 0.2) : float3(0.9, 0.2, 0.0)) * shade;
//#if GLOWPULSE
//	color += (game_time.x > 21 || game_time.x < 4 ? float3(0.0, 0.2, 0.5) : float3(0.9, 0.2, 0.0)) * shade * noiseNixie((uv + float2(timers.y * 0.2, 0.0)) * 2.5 + float2(0.5, 0.0));
//#endif

#ifdef SHOW_GRID
    float grid = 0.5 - max(abs(fmod(uva.x * 64.0, 1.0) -0.3), abs(fmod(uva.y * 64.0, 1.0) -0.3));
    float mixing = smoothstep(0.0, 64.0 / screen_res.y, grid);
    color *= 0.25 + float3(mixing, mixing, mixing);
#endif
	
	return color;
}


float3 CardioGraph(float2 uv)
{
    float persist = 0.9; // Persistance of the light
    float health = watch_actor_params.x; // Здоровье, от него зависит дуга
    float power = watch_actor_params.y; // Стамина, от нее зависит частота сердцебиения
    float speed = 0.6;

    float mx = max(screen_res.x, screen_res.y);
    float2 scrs = screen_res.xy / mx;
    uv.y = 1.0 - uv.y;
    uv = (uv / screen_res.zw) / mx;

    float f = 70.0; // Frequency of the sinus function
    float scrs_uv = uv.x - scrs.x / 2.0;

    float scrs_uv_f = scrs_uv * f;
    float uv_x_f = uv.x * f;

    float heartBeat_distance = distance(uv, float2(uv.x, (sin(scrs_uv_f) / (scrs_uv * 3.5 * f) + sin(uv_x_f) / 7.0) * clamp(1.0 - abs(scrs_uv * 10.0), 0.0, 1.0 * health) + scrs.y / 2.2));

    float heartBeat_smoothstep = 1.0 - smoothstep(0.0, 0.01, heartBeat_distance);
    float3 heartBeat = float3(heartBeat_smoothstep, heartBeat_smoothstep, heartBeat_smoothstep);

    float persist_uv = uv.x + persist - frac(timers.x * speed) * (1.0 + persist);
    float distance_uv = distance(0.0, clamp(persist_uv, 0.0, 1.0) / persist);
    float light_uv = smoothstep(0.0, 1.0, distance_uv) * (1.0 - step(1.0, distance_uv));
    float scanlignes_uv = 0.7;

    float3 light = float3(light_uv, light_uv, light_uv);
    float3 scanlignes = float3(scanlignes_uv, scanlignes_uv, scanlignes_uv);

    float3 colorRed = float3(1.0, 0.0, 0.0);
    float3 colorGreen = float3(0.0, 1.0, 0.0);
    float3 color = lerp(colorRed, colorGreen, health);

    float grid = 0.5 - max(abs(fmod(uv.x * 16.0, 1.0) - 0.4), abs(fmod(uv.y * 16.0, 1.0) - 0.4));
    float mixing = smoothstep(0.0, 8.0 / screen_res.y, grid);
    float3 grid_color = float3(mixing, mixing, mixing) * 0.75;
    color *= 0.25 + grid_color;

    float3 final = float3(heartBeat * light + scanlignes * 0.1) * color;

    return final;
}

float4 CardioGraph2(float2 uv)
{
    uv.y = 1.0 - uv.y;
    uv *= SCALE;
    uv.y -= .5 * SCALE; // Center the Y axis
    uv.x *= screen_res.x / screen_res.y; // Keeps the aspect ratio
    uv.y /= screen_res.y / screen_res.x; // Keeps the aspect ratio

    float3 colorRed = float3(1.0f, 0.0f, 0.0f);
    float3 colorGreen = float3(0.0f, 1.0f, 0.0f);
    float3 colormix = lerp(colorRed, colorGreen, watch_actor_params.x);

    float grid = GridLines(uv.x, 6.0f) + GridLines(uv.y, 6.0f);
    float3 color = (colormix * 0.1f) * grid;

    float dotX = getDotXPosition();
    float2 dotPosition = float2(dotX, generateEGC(dotX));

    color += MovingDot(uv, dotPosition);

    float delayedX = dotX;

    for(int i = 1; i < MAX_TRAIL_ITEMS; i++)
    {
        delayedX -= 0.002f;
        float2 trailPosition = float2(delayedX, generateEGC(delayedX));

        float trail = Circle(uv, trailPosition, 0.028f, 0.1f);
        float trailBlur = Circle(uv, trailPosition, 0.06f, 0.5f);

        float q = 1.0f - remap01(float(i), 1.0f, float(MAX_TRAIL_ITEMS));

        color += (colormix * q) * trail;
        color += trailBlur * (colormix * q);
    }

    return float4(color, 1.0f);
}