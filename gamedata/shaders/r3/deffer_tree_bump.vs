/**
 * @ Version: SCREEN SPACE SHADERS - UPDATE 19
 * @ Description: Trees - Trunk ( Burn Sections )
 * @ Modified time: 2023-12-16 13:42
 * @ Author: https://www.moddb.com/members/ascii1457
 * @ Mod: https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders
 */

#include "common.h"
#include "check_screenspace.h"

uniform float3x4 m_xform;
uniform float3x4 m_xform_v;
uniform float4 consts; // {1/quant,1/quant,???,???}
uniform float4 c_scale, c_bias, wind, wave;
uniform float2 c_sun; // x=*, y=+

////////////////
float4 consts_old;
float4 wave_old;
float4 wind_old;
////////////////

#ifdef SSFX_WIND
#include "screenspace_wind.h"
#endif

v2p_bumped main(v_tree I)
{
    I.Nh = unpack_D3DCOLOR(I.Nh);
    I.T = unpack_D3DCOLOR(I.T);
    I.B = unpack_D3DCOLOR(I.B);

    // Transform to world coords
    float3 pos = mul(m_xform, I.P);
    float H = pos.y - m_xform._24; // height of vertex

#ifndef SSFX_WIND
    float dp = calc_cyclic(wave.w + dot(pos, (float3)wave));
    float frac = I.tc.z * consts.x; // fractional (or rigidity)
    float inten = H * dp; // intensity
    float2 wind_result = calc_xz_wave(wind.xz * inten, frac);
# ifdef USE_TREEWAVE
    wind_result = 0;
# endif
    float4 w_pos = float4(pos.x + wind_result.x, pos.y, pos.z + wind_result.y, 1);

    //////////////
    // prev
    dp = calc_cyclic(wave_old.w + dot(pos, (float3)wave_old));
    frac = I.tc.z * consts_old.x;
    inten = H * dp;
    float2 result = calc_xz_wave(wind_old.xz * inten, frac);
# ifdef USE_TREEWAVE
    result = 0;
# endif
    float4 w_pos_previous = float4(pos.x + result.x, pos.y, pos.z + result.y, 1);
    //////////
#else
    wind_setup wset = ssfx_wind_setup();
    float2 wind_result = ssfx_wind_tree_trunk(pos, H, wset).xy;
    float4 w_pos = float4(pos.x + wind_result.x, pos.y, pos.z + wind_result.y, 1);

    wind_setup wset_old = ssfx_wind_setup(true);
    float2 wind_result_old = ssfx_wind_tree_trunk(pos, H, wset_old, true).xy;
    float4 w_pos_previous = float4(pos.x + wind_result_old.x, pos.y, pos.z + wind_result_old.y, 1);
#endif

    float2 tc = (I.tc * consts).xy;
    float hemi = I.Nh.w * c_scale.w + c_bias.w;

    // Eye-space pos/normal
    v2p_bumped O;
    float3 Pe = mul(m_V, w_pos);
    O.tcdh = float4(tc.xyyy);
    O.hpos = mul(m_VP, w_pos);

    //////////
    O.hpos_old = mul(m_VP_old, w_pos_previous);
    O.hpos_curr = O.hpos;
    O.hpos.xy = get_taa_jitter(O.hpos);
    /////////////

    O.position = float4(Pe, hemi);

#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
    float suno = I.Nh.w * c_sun.x + c_sun.y;
    O.tcdh.w = suno; // (,,,dir-occlusion)
#endif

    // Calculate the 3x3 transform from tangent space to eye-space
    // TangentToEyeSpace = object2eye * tangent2object
    //		     = object2eye * transpose(object2tangent) (since the inverse of a rotation is its transpose)
    // Normal mapping
    float3 N = unpack_bx2(I.Nh); // just scale (assume normal in the -.5f, .5f)
    float3 sphereOffset = float3(0.1, 1.0, 0.2);
    float3 sphereScale = float3(1.0, 2.0, 1.0);
    float3 sphereN = normalize(sphereScale * I.P.xyz + sphereOffset); // Spherical normals trick
    float3 T = unpack_bx2(I.T); //
    float3 B = unpack_bx2(I.B); //
    N = normalize(N);
    B = normalize(B);
    T = normalize(T);

    // tangent basis
    float3 flatB = float3(0, 0, 1);

    if (abs(dot(sphereN, flatB)) > 0.99f)
        flatB = float3(0, 1, 0);

    float3 flatT = normalize(cross(sphereN, flatB));
    flatB = normalize(cross(sphereN, flatT));

    // foliage
    float foliageMat = 0.5; // foliage
    float foliageMask = (abs(xmaterial - foliageMat) >= 0.2) ? 1 : 0; // foliage
    N = normalize(lerp(N, sphereN, foliageMask)); // blend to foliage normals

    float3x3 xform = mul((float3x3)m_xform_v, float3x3(T.x, B.x, N.x, T.y, B.y, N.y, T.z, B.z, N.z));

    // The pixel shader operates on the bump-map in [0..1] range
    // Remap this range in the matrix, anyway we are pixel-shader limited :)
    // ...... [ 2  0  0  0]
    // ...... [ 0  2  0  0]
    // ...... [ 0  0  2  0]
    // ...... [-1 -1 -1  1]
    // issue: strange, but it's slower :(
    // issue: interpolators? dp4? VS limited? black magic?

    // Feed this transform to pixel shader
    O.M1 = xform[0];
    O.M2 = xform[1];
    O.M3 = xform[2];

#ifdef USE_TDETAIL
    O.tcdbump = O.tcdh * dt_params; // dt tc
#endif

    return O;
}
FXVS;
