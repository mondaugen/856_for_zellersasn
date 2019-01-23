#include <stdio.h>
#include <math.h>
#include "midi_util.h"

#define SYNTH_CONTROL_PITCH_FINE_MIN -0.5
#define SYNTH_CONTROL_PITCH_FINE_MAX  0.5
#define SYNTH_CONTROL_PITCH_FINE_QUANT 0.01

static float map_0_1_pitch_range(float param)
{
    float _tmp;
    _tmp = round(((SYNTH_CONTROL_PITCH_FINE_MAX 
                    - SYNTH_CONTROL_PITCH_FINE_MIN)
                / SYNTH_CONTROL_PITCH_FINE_QUANT) 
            * param)
            * SYNTH_CONTROL_PITCH_FINE_QUANT 
            + SYNTH_CONTROL_PITCH_FINE_MIN;
    return _tmp;
}    

static float test_map_midpoint_exact()
{
    printf("%f\n",midi_util_map_midpoint_exact(64,0,1));
    printf("%f\n",midi_util_map_midpoint_exact(127,0,1));
    printf("%f\n",midi_util_map_midpoint_exact(0,0,1));
    printf("%f\n",map_0_1_pitch_range(midi_util_map_midpoint_exact(64,0,1)));
    printf("%f\n",map_0_1_pitch_range(midi_util_map_midpoint_exact(127,0,1)));
    printf("%f\n",map_0_1_pitch_range(midi_util_map_midpoint_exact(0,0,1)));
    printf("\n");
    printf("%f\n",map_0_1_pitch_range(midi_util_map_midpoint_exact(63,0,1)));
    printf("%f\n",map_0_1_pitch_range(midi_util_map_midpoint_exact(65,0,1)));
    printf("%f\n",map_0_1_pitch_range(midi_util_map_midpoint_exact(66,0,1)));
}

int main(void)
{
    test_map_midpoint_exact();
    return 0;
}
