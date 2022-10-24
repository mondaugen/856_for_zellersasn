# Generate a table to look up coarse and fine tempi

import numpy as np
import os
from gen_common import complete_array, get_env

N_POINTS=get_env('N_POINTS',128,int)
MIN_TEMPO_BPM=get_env('MIN_TEMPO_BPM',10,int)
MAX_TEMPO_BPM=get_env('MAX_TEMPO_BPM',60,int)
BASE=2
# We have 2 controls: coarse tempo and fine tempo
# We therefore make a table with N_POINTS*N_POINTS points. The coarse index
# jumps by N_POINTS and the fine index jumps by 1 point
# We need an extra N_POINTS at the end in the case that the coarse index
# provided below is 1, in that case (if fine is 0), the index will be
# N_POINTS // 2 + (N_POINTS*N_POINTS). As fine varies from -0.5 to 0.5, the
# highest index possible is actually N_POINTS // 2 + (N_POINTS*N_POINTS) + N_POINTS // 2 = (N_POINTS+1)*N_POINTS
n2_points=(N_POINTS+1)*N_POINTS + 1
tempo_scale=(np.power(BASE,np.arange(n2_points)/n2_points)-1)/(BASE-1)
tempi=MIN_TEMPO_BPM+(MAX_TEMPO_BPM-MIN_TEMPO_BPM)*tempo_scale
coarse_index_offset=N_POINTS//2

assert(np.all(tempo_scale >= 0))
assert(np.all(tempo_scale < 1))

s=''

s+="""
/*
The number of indexable points for coarse or fine. Get the index by mutiplying
this by a number in [0,1) for coarse and (-0.5,0.5) for fine. This resulting
index is multiplied by COARSE_TEMPO_INDEX_STEP if addressing the coarse indices
and FINE_TEMPO_INDEX_STEP if addressing the fine indices. Then the indices are
summed to get the final index.
*/
#define TEMPO_MAP_N_POINTS %d
#define MIN_COARSE_TEMPO_INDEX_OFFSET %d
#define COARSE_TEMPO_INDEX_STEP %d
#define FINE_TEMPO_INDEX_STEP %d
""" % (N_POINTS,N_POINTS//2,N_POINTS,1)

s+="""
static const float tempo_map_table[] = {
"""
s=complete_array(tempi,s)

s+="""
/*
coarse in [0,1], fine in [0,1] 
*/
static float
tempo_map_table_lookup(float coarse, float fine)
{
    fine -= 0.5;
    int coarse_idx = MIN_COARSE_TEMPO_INDEX_OFFSET
            + COARSE_TEMPO_INDEX_STEP * ((int)TEMPO_MAP_N_POINTS*coarse),
        fine_idx = TEMPO_MAP_N_POINTS * fine;
    return tempo_map_table[coarse_idx+fine_idx];
}
"""

outfile=os.environ['OUTFILE']
with open(outfile,"w") as f:
    f.write(s)
