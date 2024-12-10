#include "common.h"

cbuffer dynamic_tree
{
uniform float3x4 m_xform;
uniform float3x4 m_xform_v;

uniform float4 consts; // {1/quant,1/quant,???,???}
uniform float4 wind;
uniform float4 wave;

uniform float4 c_scale;
uniform float4 c_bias;
uniform float2 c_sun; // x=*, y=+

//////////
float4 consts_old;
float4 wave_old;
float4 wind_old;
///////////
}

v2p_flat main(v_tree I)
{
    I.Nh = unpack_D3DCOLOR(I.Nh);
    I.T = unpack_D3DCOLOR(I.T);
    I.B = unpack_D3DCOLOR(I.B);

    v2p_flat o;

    // Transform to world coords
    float3 pos = mul(m_xform, I.P);

    //
    float base = m_xform._24; // take base height from matrix
    float dp = calc_cyclic(wave.w + dot(pos, (float3)wave));
    float H = pos.y - base; // height of vertex (scaled, rotated, etc.)
    float frac = I.tc.z * consts.x; // fractional (or rigidity)
    float inten = H * dp; // intensity
    float2 result = calc_xz_wave(wind.xz * inten, frac);
#ifdef USE_TREEWAVE
    result = 0;
#endif
    float4 f_pos = float4(pos.x + result.x, pos.y, pos.z + result.y, 1);

    ////////////////////////////
    // prev
    dp = calc_cyclic(wave_old.w + dot(pos, (float3)wave_old));
    frac = I.tc.z * consts_old.x; // fractional (or rigidity)
    inten = H * dp; // intensity
    result = calc_xz_wave(wind_old.xz * inten, frac);
#ifdef USE_TREEWAVE
    result = 0;
#endif
    float4 f_pos_previous = float4(pos.x + result.x, pos.y, pos.z + result.y, 1);
    ////////////////////////////

    // Normal mapping
    float3 N = unpack_bx2(I.Nh);
    float3 sphereOffset = float3(0.0, 1.0, 0.0);
    float3 sphereScale = float3(1.0, 2.0, 1.0);
    float3 sphereN = normalize(sphereScale * I.P.xyz + sphereOffset); // Spherical normals trick
    float3 flatN = (float3(0, 1, 0));
    /*
    float3 camFacingN = normalize((f_pos - eye_position.xyz) * float3(-1,0,-1));
    sphereN = lerp(camFacingN, sphereN, saturate(H)); //roots face the camera, the tips face the sky

    sphereN.xz *= 0.5;
    sphereN.y = sqrt(1 - saturate(dot(sphereN.xz, sphereN.xz)));
    sphereN = normalize(sphereN);
    */
    // foliage
    float foliageMat = 0.5; // foliage
    // float foliageMask = saturate(abs(xmaterial-foliageMat)-0.02); //foliage
    float foliageMask = (abs(xmaterial - foliageMat) >= 0.2) ? 1 : 0; // foliage
    // float foliageMask = 1; //foliage
    N = normalize(lerp(N, sphereN, foliageMask)); // blend to foliage normals

    // Final xform(s)
    // Final xform
    float3 Pe = mul(m_V, f_pos);
    // float3 Pe = mul(m_V, float4(pos.xyz,1));
    float hemi = I.Nh.w * c_scale.w + c_bias.w;
    // float hemi 	= I.Nh.w;
    o.hpos = mul(m_VP, f_pos);

    ////////////////////
    o.hpos_old = mul(m_VP_old, f_pos_previous);
    o.hpos_curr = o.hpos;
    o.hpos.xy = get_taa_jitter(o.hpos);
    ///////////////////

    o.N = mul((float3x3)m_xform_v, N);
    o.tcdh = float4((I.tc * consts).xyyy);
    o.position = float4(Pe, hemi);

#ifdef USE_GRASS_WAVE
    o.tcdh.z = 1.f;
#endif

#ifdef USE_TDETAIL
    o.tcdbump = o.tcdh * dt_params; // dt tc
#endif

    return o;
}
FXVS;
