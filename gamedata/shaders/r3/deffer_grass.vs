#define SSFX_WIND_ISGRASS

#include "common.h"
#include "check_screenspace.h"

float4 benders_pos[32];
float4 benders_pos_old[32];
float4 benders_setup;

float4 consts; // {1/quant,1/quant,diffusescale,ambient}
float4 wave; // cx,cy,cz,tm
float4 dir2D;
float4 array[61 * 4];

////////////////
uniform float4 consts_old;
uniform float4 dir2D_old;
uniform float4 wave_old;
////////////////

#ifdef SSFX_WIND
#include "screenspace_wind.h"
#endif

v2p_bumped main(v_detail v)
{
    v2p_bumped O;
    // index
    int i = v.misc.w;
    float4 m0 = array[i + 0];
    float4 m1 = array[i + 1];
    float4 m2 = array[i + 2];
    float4 c0 = array[i + 3];

    // Transform pos to world coords
    float4 P;
    P.x = dot(m0, v.pos);
    P.y = dot(m1, v.pos);
    P.z = dot(m2, v.pos);
    P.w = 1;

    float H = P.y - m1.w; // height of vertex (scaled)

#ifndef SSFX_WIND
    float dp = calc_cyclic(dot(P, wave));
    float frac = v.misc.z * consts.x; // fractional
    float inten = H * dp;
    float2 result = calc_xz_wave(dir2D.xz * inten, frac);
    float4 pos = float4(P.x + result.x, P.y, P.z + result.y, 1);

    // prev
    dp = calc_cyclic(dot(P, wave_old));
    frac = v.misc.z * consts_old.x;
    inten = H * dp;
    result = calc_xz_wave(dir2D_old.xz * inten, frac);
    float4 w_pos_previous = float4(P.x + result.x, P.y, P.z + result.y, 1);
#else
    wind_setup wset = ssfx_wind_setup();
    float3 wind_result = ssfx_wind_grass(P.xyz, H, wset);
    float4 pos = float4(P.xyz + wind_result.xyz, 1);

    wind_setup wset_old = ssfx_wind_setup(true);
    float3 wind_result_old = ssfx_wind_grass(P.xyz, H, wset_old, true);
    float4 w_pos_previous = float4(P.xyz + wind_result_old.xyz, 1);
#endif

    // INTERACTIVE GRASS - SSS Update 15.4
    // https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders/
#ifdef SSFX_INTER_GRASS
    for (int b = 0; b < benders_setup.w; b++)
    {
        // Direction, Radius & Bending Strength, Distance and Height Limit
        float3 dir = benders_pos[b + 16].xyz;
        float3 rstr = float3(benders_pos[b].w, benders_pos[b + 16].ww);
        bool non_dynamic = rstr.x <= 0 ? true : false;
        float dist = distance(pos.xz, benders_pos[b].xz);
        float height_limit = 1.0f - saturate(abs(pos.y - benders_pos[b].y) / (non_dynamic ? 2.0f : rstr.x));
        height_limit *= H;

        // Adjustments ( Fix Radius or Dynamic Radius )
        rstr.x = non_dynamic ? benders_setup.x : rstr.x;
        rstr.yz *= non_dynamic ? benders_setup.yz : 1.0f;

        // Strength through distance and bending direction.
        float bend = 1.0f - saturate(dist / (rstr.x + 0.001f));
        float3 bend_dir = normalize(pos.xyz - benders_pos[b].xyz) * bend;
        float3 dir_limit = dir.y >= -1 ? saturate(dot(bend_dir.xyz, dir.xyz) * 5.0f) : 1.0f; // Limit if nedeed

        // Apply direction limit
        bend_dir.xz *= dir_limit.xz;

        // Apply vertex displacement
        pos.xz += bend_dir.xz * 2.0f * rstr.yy * height_limit; // Horizontal
        pos.y -= bend * 0.6f * rstr.z * height_limit * dir_limit.y; // Vertical
    }

    //for old frame
    for (int b = 0; b < benders_setup.w; b++)
    {
        // Direction, Radius & Bending Strength, Distance and Height Limit
        float3 dir = benders_pos_old[b + 16].xyz;
        float3 rstr = float3(benders_pos_old[b].w, benders_pos_old[b + 16].ww);
        bool non_dynamic = rstr.x <= 0 ? true : false;
        float dist = distance(w_pos_previous.xz, benders_pos_old[b].xz);
        float height_limit = 1.0f - saturate(abs(w_pos_previous.y - benders_pos_old[b].y) / (non_dynamic ? 2.0f : rstr.x));
        height_limit *= H;

        // Adjustments ( Fix Radius or Dynamic Radius )
        rstr.x = non_dynamic ? benders_setup.x : rstr.x;
        rstr.yz *= non_dynamic ? benders_setup.yz : 1.0f;

        // Strength through distance and bending direction.
        float bend = 1.0f - saturate(dist / (rstr.x + 0.001f));
        float3 bend_dir = normalize(w_pos_previous.xyz - benders_pos_old[b].xyz) * bend;
        float3 dir_limit = dir.y >= -1 ? saturate(dot(bend_dir.xyz, dir.xyz) * 5.0f) : 1.0f; // Limit if nedeed

        // Apply direction limit
        bend_dir.xz *= dir_limit.xz;

        // Apply vertex displacement
        w_pos_previous.xz += bend_dir.xz * 2.0f * rstr.yy * height_limit; // Horizontal
        w_pos_previous.y -= bend * 0.6f * rstr.z * height_limit * dir_limit.y; // Vertical
    }
#endif

    // FLORA FIXES & IMPROVEMENTS - SSS Update 14.6
    // https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders/
    // Fake Normal, Bi-Normal and Tangent
    float3 N = normalize(float3(P.x - m0.w, P.y - m1.w + 1.0f, P.z - m2.w));

    float3x3 xform = mul((float3x3)m_WV, float3x3(0, 0, N.x, 0, 0, N.y, 0, 0, N.z));

    // Feed this transform to pixel shader
    O.M1 = xform[0];
    O.M2 = xform[1];
    O.M3 = xform[2];

    // Eye-space pos/normal
    float hemi = clamp(c0.w, 0.05f, 1.0f); // Some spots are bugged ( Full black ), better if we limit the value till a better solution. // Option -> v_hemi(N);
    float3 Pe = mul(m_V, pos);
    O.tcdh = float4((v.misc * consts).xyyy);
    O.hpos = mul(m_VP, pos);

    /////////////
    O.hpos_old = mul(m_VP_old, w_pos_previous);
    O.hpos_curr = O.hpos;
    O.hpos.xy = get_taa_jitter(O.hpos);
    /////////////

    O.position = float4(Pe, hemi);

#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
    O.tcdh.w = hemi * c_sun.x + c_sun.y; // (,,,dir-occlusion)
#endif

    return O;
}
FXVS;
