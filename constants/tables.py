import math

DELTA_TIME_FREE_TAB_LEN=65
dtf_pts=[x/float(DELTA_TIME_FREE_TAB_LEN-1) for x in
        xrange(DELTA_TIME_FREE_TAB_LEN)]
dtf_vls=[(math.pow(2.,-6*(1.-x)) - math.pow(2.,-6))/(1. - math.pow(2.,-6.))
            for x in dtf_pts]

with open('constants/tables.c','w') as f:
    f.write("const float tables_delta_time_free[] = {\n")
    for x in dtf_vls:
        f.write("    %f,\n" % (x,))
    f.write("};\n")

with open('constants/tables.h','w') as f:
    f.write("#ifndef TABLES_H\n#define TABLES_H\n")
    f.write("extern const float tables_delta_time_free[];\n")
    f.write("#endif /* TABLES_H */\n")
