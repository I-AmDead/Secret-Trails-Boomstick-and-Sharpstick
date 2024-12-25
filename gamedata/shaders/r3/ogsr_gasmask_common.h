/*
    Gasmask overlay shader

    Credits: ZHC (visor reflection)
    /////////////////
    Anomaly Team 2020
    /////////////////
*/

// Main gasmask textures
Texture2D s_mask_nm_1;
Texture2D s_mask_nm_2;
Texture2D s_mask_nm_3;
Texture2D s_mask_nm_4;
Texture2D s_mask_nm_5;
Texture2D s_mask_nm_6;
Texture2D s_mask_nm_7;
Texture2D s_mask_nm_8;
Texture2D s_mask_nm_9;
Texture2D s_mask_nm_10;

// Breath noise texture
Texture2D s_breath_noise;

// RT including drops
Texture2D s_image_blurred;

// Constants from engine
uniform float4 mask_control;
uniform float3 drops_control;

uniform float breath_size;
uniform float breath_idx;
uniform float4 raindrop_restart;

// Gasmask settings
#define GM_DIST_INT 0.05 // Refraction intensity
#define GM_DIFF_INT 0.25 // Diffuse cracks intensity
#define GM_VIG_INT 0.5 // Vignette intensity

// Raindrops settings
#define GM_DROPS_TURBSIZE 15.0 // Turbulence power
#define GM_DROPS_TURBSHIFT float4(0.35, 1.0, 0.0, 1.0) // Turbulence offset
#define GM_DROPS_TURBTIME sin(0.1 / 3.0)
#define GM_DROPS_TURBCOF 0.33 // Turbulence intensity

// Glass reflections settings
#define GM_VIS_NUM 16 // Reflection quality
#define GM_VIS_RADIUS 0.45 // Reflection radius
#define GM_VIS_INTENSITY 0.5 // Reflection intensity

float resize(float input, float factor, float offset) { return (input - 0.5f + offset) / factor + 0.5f - offset; }