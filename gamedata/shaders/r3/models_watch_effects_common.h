uniform float4 game_time;
uniform float4 watch_actor_params;
uniform float4 watch_color_params;
uniform float4 radiation_effect;

#define PI 3.14159265359
#define TPI 6.28318530718

#define timerX10 (timers.z * 10.0)

#define tt (timerX10 * 0.1)
#define ft (frac(tt))
#define it (floor(tt))

#define THICCNESS 0.01
#define RADIUS 0.15
#define SPEED 10.0

#define aa 2.0 / min(screen_res.x, screen_res.y)

// Hearthbeat
#define SCALE 1.0
#define LINES_WIDTH 0.05
#define DOT_SPEED_LIMITER 2.4
#define MAX_TRAIL_ITEMS 600

// NixieTime
#define TWELVE_HOUR_CLOCK 1
#define GLOWPULSE 1
#define SHOW_GRID

float resize(float input, float factor, float offset) { return (input - 0.5f + offset) / factor + 0.5f - offset; }

float2x2 rotate(float r)
{
    float c = cos(r);
    float s = sin(r);
    return float2x2(c, -s, s, c);
}
float2 rotate(float2 p, float r) { return mul(rotate(r), p); }

float3 rotate(float3 p, float3 r)
{
    p.xy = rotate(p.xy, r.z);
    p.yz = rotate(p.yz, r.x);
    p.zx = rotate(p.zx, r.y);
    return p;
}

float radiansToDegrees(float radians_value) { return radians_value * (180.0 / PI); }

float3 hsv(float3 c)
{
    float3 rgb = c.x * 6. + float3(0.0, 4.0, 2.0);
    rgb = fmod(rgb, 6.0) - 3.0;
    rgb = abs(rgb) - 1.0;
    rgb = clamp(rgb, 0.0, 1.0);

    return mul(lerp(float3(1.0, 1.0, 1.0), rgb, c.y), c.z);
}

// returns 0 to 1 arc(turns of a circle) with 0 and 1 at the right
float hue(float2 p) { return frac(atan2(p.y, p.x) / TPI + 1.); }

float stepLinear(float a, float b, float t) { return clamp((t - a) / (b - a), 0., 1.); }

// originally https://www.shadertoy.com/view/ltl3DB
float easeSpring(float a, float b, float t)
{
    t = stepLinear(a, b, t);
    return 1.0 - cos(t * TPI * 3.0) * exp(-t * 12.5);
}

float easeInOutExpo(float t) { return (t == 0. || t == 1.) ? t : (t *= 2.) < 1. ? 0.5 * pow(2., 10. * (t - 1.)) : 0.5 * (-pow(2., -10. * (t - 1.)) + 2.); }

// iquilez - https://www.shadertoy.com/view/wl23RK
// sc is the sin/cos of the aperture
float sdArc(float2 p, float2 sc, float ra, float rb)
{
    p.x = abs(p.x);
    return ((sc.y > sc.x) ? length(p - sc * ra) : abs(length(p) - ra)) - rb;
}

float hash(float v) { return frac(sin(v) * 43758.5453); }
float hash(float2 v) { return frac(sin(dot(v, float2(12.9898, 78.233))) * 43758.5453); }
float noise_hash(float v)
{
    float f = frac(v), i = floor(v), u = f * f * (3.0 - 2.0 * f);
    return lerp(hash(i), hash(i + 1.0), u);
}
float noise_hash(float2 v)
{
    float2 f = frac(v), i = floor(v), u = f * f * (3.0 - 2.0 * f);
    return lerp(lerp(hash(i + float2(0.0, 0.0)), hash(i + float2(1.0, 0.0)), u.x), lerp(hash(i + float2(0.0, 1.0)), hash(i + float2(1.0, 1.0)), u.x), u.y);
}
float noise_hash(float3 v)
{
    float3 f = frac(v), i = floor(v), u = f * f * (3.0 - 2.0 * f);
    float n = i.x + i.y * 53.0 + i.z * 117.0;
    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), u.x), lerp(hash(n + 53.0), hash(n + 54.0), u.x), u.y),
                lerp(lerp(hash(n + 117.0), hash(n + 118.0), u.x), lerp(hash(n + 170.0), hash(n + 171.0), u.x), u.y), u.z);
}
float3 random3(float3 c)
{
    float j = 4096.0 * sin(dot(c, float3(17.0, 59.4, 15.0)));
    float3 r;
    r.z = frac(512.0 * j);
    j *= 0.125;
    r.x = frac(512.0 * j);
    j *= 0.125;
    r.y = frac(512.0 * j);
    return r - 0.5;
}

float3 glitch(float3 p, float seed)
{
    float hs = hash(seed);
    float3 q = p;
    [unroll] for (int i = 0; i < 4; i++)
    {
        float fi = float(i) + 1.0;
        q = p * 2.0 * fi;
        float3 iq = floor(q);
        float3 fq = frac(q);
        float n = noise_hash(rotate(iq, float3(hs, hs, hs)));
        float3 offset = 3.0 * random3(float3(n, n, n) * float3(10.486, 78.233, 65.912));
        if (hash(n) < 0.1)
        {
            p = p + offset;
            break;
        }
    }
    return p;
}
float3 glitch(float3 p) { return glitch(p, 43768.5453); }

float grid(float2 uv, float n, float w)
{
    uv = frac(uv * n);
    uv = abs(uv - 0.5);
    return 1.0 - smoothstep(-0.5 * w, 0.5 * w, min(uv.x, uv.y));
}

float box(float3 p, float3 b)
{
    float3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}
float box(float3 p, float b) { return box(p, float3(b, b, b)); }

float2 minD(float2 d1, float2 d2) { return d1.x < d2.x ? d1 : d2; }

float3 transform(float3 p)
{
    p = rotate(p, float3(0.25 * PI, 0.25 * PI, 0.0));
    return p;
}

#define IS_GLITCH (noise_hash(timers.z * 12.0) < 0.6)

float2 map(float3 p)
{
    float2 d = float2(1e4, -1.0);

    float3 q1 = transform(p);
    float tn = 10.0 * hash(floor(timerX10 * 2.0) / 2.0);
    float qs = floor(timerX10 * tn) / tn;
    float3 q2 = glitch(q1, qs);

    float r = 0.5;
    float2 b = IS_GLITCH ? float2(box(q2, float3(r, r, r)), 2.0) : float2(box(q1, float3(r, r, r)), 1.0);
    d = minD(d, b);

    return d;
}

float2 trace(float3 ro, float3 rd, float2 tmm)
{
    float t = tmm.x;
    float m = -1.0;
    for (int i = 0; i < 200; i++)
    {
        float2 d = map(ro + rd * t);
        if (d.x < 1e-4 || tmm.y < t)
            break;
        t += d.x * 0.5;
        m = d.y;
    }
    if (tmm.y < t)
        m = -1.0;
    return float2(t, m);
}

float3 calcNormal(float3 p)
{
    float2 e = float2(1.0, -1.0) * 1.0 - 4.0;
    return normalize(e.xyy * map(p + e.xyy).x + e.yxy * map(p + e.yxy).x + e.yyx * map(p + e.yyx).x + e.xxx * map(p + e.xxx).x);
}

float3 render(float3 ro, float3 rd, float2 uv)
{
    float3 col = float3(0.0, 0.0, 0.0);
    float2 cmm = float2(0.0, 30.0);
    float2 res = trace(ro, rd, cmm);
    float t = res.x;
    float m = res.y;
    if (m < 0.0)
    {
        col = float3(0.0, 0.0, 0.0);
    }
    else
    {
        float3 pos = ro + rd * t;
        float3 nor = calcNormal(pos);
        float3 ref = reflect(rd, nor);
        float3 opos = transform(pos);

        float w = 0.2;
        float n = 5.0;
        float coll = grid(opos.xy + 0.5, n, w) + grid(opos.yz + 0.5, n, w);
        col = float3(coll, coll, coll);
        col = clamp(pow(abs(col * 2.0), float3(1.4, 1.4, 1.4)), 0.0, 1.0);
        if (IS_GLITCH)
            col *= float3(3.0 * noise_hash(floor(glitch(opos))), 0.0, 0.0);
    }
    return col;
}

float circle(float2 uv, float2 pos, float rad) { return 1.0 - smoothstep(rad, rad + 0.005, length(uv - pos)); }

float ring(float2 uv, float2 pos, float innerRad, float outerRad)
{
    return (1.0 - smoothstep(outerRad, outerRad + aa, length(uv - pos))) * smoothstep(innerRad - aa, innerRad, length(uv - pos));
}

//
// NixieTime
//

// hash function copy from https://www.shadertoy.com/view/4djSRW
float hash12(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.x + p3.y) * p3.z);
}

float noiseNixie(float2 pos)
{
    float2 i = floor(pos);
    float2 f = frac(pos);

    float a = hash12(i);
    float b = hash12(i + float2(1, 0));
    float c = hash12(i + float2(0, 1));
    float d = hash12(i + float2(1, 1));

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

// Distance to a line segment,
float dfLine(float2 start, float2 end, float2 uv)
{
    start = mul(start, 1.0 / 6.0);
    end = mul(end, 1.0 / 6.0);

    float2 linessss = end - start;
    float fraccccc = dot(uv - start, linessss) / dot(linessss, linessss);
    return distance(start + linessss * clamp(fraccccc, 0.0, 1.0), uv);
}

// Distance to the edge of a circle.
float dfCircle(float2 origin, float radius, float2 uv)
{
    origin = mul(origin, 1.0 / 6.0);
    radius = mul(radius, 1.0 / 6.0);

    return abs(length(uv - origin) - radius);
}

// Distance to an arc.
float dfArc(float2 origin, float start, float sweep, float radius, float2 uv)
{
    origin = mul(origin, 1.0 / 6.0);
    radius = mul(radius, 1.0 / 6.0);

    uv -= origin;
    uv = mul(uv, float2x2(cos(start), -sin(start), sin(start), cos(start)));

    float offs = (sweep / 2.0 - PI);
    float ang = fmod(atan2(uv.y, uv.x) - offs, TPI) + offs;
    ang = clamp(ang, min(0.0, sweep), max(0.0, sweep));

    return distance(mul(radius, float2(cos(ang), sin(ang))), uv);
}

// Distance to the digit "d" (0-9).
float dfDigit(float2 origin, float d, float2 uv)
{
    uv -= origin;
    d = floor(d);
    float dist = 1e6;

    if (d == 0.0)
    {
        float distsss = 1e6;
        distsss = min(distsss, dfLine(float2(1.000, 1.000), float2(1.000, 0.500), uv));
        distsss = min(distsss, dfLine(float2(0.000, 1.000), float2(0.000, 0.500), uv));
        distsss = min(distsss, dfArc(float2(0.500, 1.000), 0.000, 3.142, 0.500, uv));
        distsss = min(distsss, dfArc(float2(0.500, 0.500), 3.142, 3.142, 0.500, uv));

        return distsss;
    }

    if (d == 1.0)
    {
        dist = min(dist, dfLine(float2(0.500, 1.500), float2(0.500, 0.000), uv));
        return dist;
    }

    if (d == 2.0)
    {
        dist = min(dist, dfLine(float2(1.000, 0.000), float2(0.000, 0.000), uv));
        dist = min(dist, dfLine(float2(0.388, 0.561), float2(0.806, 0.719), uv));
        dist = min(dist, dfArc(float2(0.500, 1.000), -3.100, -3.142, 0.500, uv));
        dist = min(dist, dfArc(float2(0.700, 1.000), 5.074, 1.209, 0.300, uv));
        dist = min(dist, dfArc(float2(0.600, 0.000), 1.932, 1.209, 0.600, uv));
        return dist;
    }

    if (d == 3.0)
    {
        dist = min(dist, dfLine(float2(0.000, 1.500), float2(1.000, 1.500), uv));
        dist = min(dist, dfLine(float2(1.000, 1.500), float2(0.500, 1.000), uv));
        dist = min(dist, dfArc(float2(0.500, 0.500), -4.742, -4.712, 0.500, uv));
        return dist;
    }

    if (d == 4.0)
    {
        dist = min(dist, dfLine(float2(0.700, 1.500), float2(0.000, 0.500), uv));
        dist = min(dist, dfLine(float2(0.000, 0.500), float2(1.000, 0.500), uv));
        dist = min(dist, dfLine(float2(0.700, 1.200), float2(0.700, 0.000), uv));
        return dist;
    }

    if (d == 5.0)
    {
        dist = min(dist, dfLine(float2(1.000, 1.500), float2(0.300, 1.500), uv));
        dist = min(dist, dfLine(float2(0.300, 1.500), float2(0.200, 0.900), uv));
        dist = min(dist, dfArc(float2(0.500, 0.500), -4.074, -5.356, 0.500, uv));
        return dist;
    }

    if (d == 6.0)
    {
        dist = min(dist, dfLine(float2(0.067, 0.750), float2(0.500, 1.500), uv));
        dist = min(dist, dfCircle(float2(0.500, 0.500), 0.500, uv));
        return dist;
    }

    if (d == 7.0)
    {
        dist = min(dist, dfLine(float2(0.000, 1.500), float2(1.000, 1.500), uv));
        dist = min(dist, dfLine(float2(1.000, 1.500), float2(0.500, 0.000), uv));
        return dist;
    }

    if (d == 8.0)
    {
        dist = min(dist, dfCircle(float2(0.500, 0.400), 0.400, uv));
        dist = min(dist, dfCircle(float2(0.500, 1.150), 0.350, uv));
        return dist;
    }

    if (d == 9.0)
    {
        dist = min(dist, dfLine(float2(0.933, 0.750), float2(0.500, 0.000), uv));
        dist = min(dist, dfCircle(float2(0.500, 1.000), 0.500, uv));
        return dist;
    }

    return dist;
}

// Distance to a number This handles 2 digit integers, leading 0's will be drawn
float dfNumber(float2 origin, float num, float2 uv)
{
    uv -= origin;
    float dist = 1.0;
    float offs = 0.0;

    float2 digit_spacing = mul(float2(1.1, 1.6), 1.0 / 6.0);

    for (float i = 1.0; i >= 0.0; i--)
    {
        float d = fmod(num / pow(10.0, i), 10.0);

        float2 pos = digit_spacing * float2(offs, 0.0);

        dist = min(dist, dfDigit(pos, d, uv));
        offs++;
    }
    return dist;
}

// Distance to a number This handles 2 digit integers, leading 0's will be drawn
float dfNumber2(float2 origin, float num, float2 uv)
{
    uv -= origin;
    float dist = 1.0;
    float offs = 0.0;

    float2 digit_spacing = mul(float2(1.3, 1.6), 1.0 / 6.0);

    for (float i = 3.0; i >= 0.0; i--)
    {
        float d = fmod(num / pow(10.0, i), 10.0);

        float2 pos = digit_spacing * float2(offs, 0.0);

        dist = min(dist, dfDigit(pos, d, uv));
        offs++;
    }
    return dist;
}

float dfColon(float2 origin, float2 uv)
{
    uv -= origin;
    float dist = 1.0;
    float offs = 0.0;

    dist = min(dist, dfCircle(float2(offs + 0.9, 0.9) * 1.1, 0.04, uv));
    dist = min(dist, dfCircle(float2(offs + 0.9, 0.4) * 1.1, 0.04, uv));

    return dist;
}

// Length of a number in digits
float numberLength(float n) { return floor(max(log(n) / log(10.0), 0.0) + 1.0) + 2.0; }

//
// HearthBeat
//

float remap01(float t, float a, float b) { return (t - a) / (b - a); }

float Circle(float2 uv, float2 position, float radius, float blur)
{
    float distance = length(uv - position);
    return smoothstep(radius, radius - blur, distance);
}

float GridLines(float t, float lines) { return step(frac(t * lines), LINES_WIDTH); }

float3 Ring(float2 uv, float2 position)
{
    float ring = Circle(uv, position, 0.08f, 0.01f);
    ring -= Circle(uv, position, 0.065f, 0.01f);

    float3 RING_COLOR = float3(0.0f, 0.01f, 0.0f);

    return RING_COLOR * ring;
}

float spike(float x, float d, float w, float raiseBy)
{
    float f1 = pow(abs(x + (d * SCALE)), raiseBy);
    return exp(-f1 / w);
}

float generateEGC(float x)
{
    x -= .7 * SCALE;

    float a = 0.4 * SCALE;
    float d = .3;
    float w = 0.001;

    float f1 = a * spike(x, d, w, 2.);
    float f2 = 0.3 * spike(x, d - 0.02, 0.5 * w, 2.0);
    float f3 = 0.1 * spike(x, d - 0.25, 0.0002, 2.5);
    float f4 = 0.07 * spike(x, d - 0.55, 0.009, 1.5);
    float f5 = 0.1 * spike(x, d - 0.75, 0.001, 2.7);

    float f6 = a * spike(x, d - 1., 0.002, 2.);
    float f7 = 0.3 * spike(x, d - 1.025, 0.5 * w, 2.0);
    float f8 = 0.1 * spike(x, d - 1.2, 0.0002, 2.5);
    float f9 = 0.07 * spike(x, d - 1.55, 0.009, 1.3);

    return (f1 - f2 + f3 + f4 + f5 + f6 - f7 + f8 + f9) * watch_actor_params.x;
    ;
}

float getDotXPosition()
{
    float dotX = frac(timers.x / DOT_SPEED_LIMITER);
    dotX *= 2.0f * SCALE;
    return dotX;
}

float3 MovingDot(float2 uv, float2 dotPosition)
{
    float movingDot = Circle(uv, dotPosition, 0.015f, 0.01f);
    float smallBlurredDot = Circle(uv, dotPosition, 0.06f, 0.1f);
    float bigBlurredDot = Circle(uv, dotPosition, 0.3f, 0.6f);

    float3 colorRed = float3(1.0f, 0.0f, 0.0f);
    float3 colorGreen = float3(0.0f, 1.0f, 0.0f);
    float3 colormix = lerp(colorRed, colorGreen, watch_actor_params.x);

    float3 color = colormix * 0.1f * movingDot;
    color += colormix * smallBlurredDot;
    color += colormix * bigBlurredDot;
    color += Ring(uv, dotPosition);

    return color;
}