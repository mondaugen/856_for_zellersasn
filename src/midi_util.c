float midi_util_map_midpoint_exact(float mdata, float x0, float x1)
{
    float m = (x0 + x1)* 0.5;
    if (mdata == 64) { return m; }
    if (mdata > 64) { return (x1 - m) * (mdata - 64) / 63. + m; }
    return (m - x0) * (mdata / 64.) + x0;
}

