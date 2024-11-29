#ifndef MBLUR_SETTINGS_H
#define MBLUR_SETTINGS_H
/////////////////////////////////////////////////////////
//    Enhanced Motion Blur Settings                    //
// Made by Dlani  Credits to original shader code devs //
/////////////////////////////////////////////////////////

#define EMBLUR_SAMPLES int(12) // Blur samples     // default int(12)
#define EMBLUR_CLAMP float(0.025) // Max blur length  // default float(0.025)  // vanilla float(0.012)
#define EMBLUR_LENGTH float(120) // Blur length      // default float(120)    // vanilla float(12)
#define EMBLUR_DITHER float(1) // Blur dither      // default float(1)

#define EMBLUR_CONE_DITHER float(0.25) // Cone like dithering aka blurriness // default float(0.25)

#define EMBLUR_DUAL // Blur in both directions. Comment to disable

#define EMBLUR_WPN // Disabled motion blur for weapon and hud. Comment to disable
#define EMBLUR_WPN_RADIUS float(1.3) // default float(1.3)
#define EMBLUR_WPN_RADIUS_SMOOTHING float(1.0) // default float(1.0)

#define EMBLUR_SIGHT_MASK // Doesn't blur sights
#define EMBLUR_SIGHT_MASK_COLOR // Color based sight detection

// More samples for color based sight detection. Controls size of the detection. From minor to significant performance impact.
#define EMBLUR_SIGHT_MASK_COLOR_EXTRA_SAMPLES 0 // Can be 0 or 4 or 8.

#define EMBLUR_SIGHT_MASK_SIZE float(5) // Radius for searching   // default float(5)
#define EMBLUR_SIGHT_MASK_COLOR_THRESHOLD float(0.4) // Threshold for masking  // default float(0.5)
#define EMBLUR_SIGHT_MASK_COLOR_CONTRAST float(10) // Multiplier for mask    // default float(10)

#endif
