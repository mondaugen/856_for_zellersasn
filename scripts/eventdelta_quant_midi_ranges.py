import re
import numpy as np
from itertools import chain
SYNTH_CONTROL_H='inc/synth_control.h'
table_pat = re.compile('SYNTH_CONTROL_TEMPOBPM_SCALE_TABLE[^\{]+\{([^\}]+)\}')
with open(SYNTH_CONTROL_H,'r') as fd:
    contents=fd.read().replace('\n',' ')
    table=[x.strip() for x in table_pat.search(contents).group(1).split(',')]
    inds=np.floor(np.arange(127)/127*len(table))
    chngs=np.where(np.diff(inds) == 1.)[0]
    chngs=np.concatenate(([0],chngs,chngs+1,[127]))
    chngs.sort()
    print('<table>')
    for row,val in chain([('CC range','scale value')],zip(chngs.reshape(-1,2),table)):
        print(
f"""<tr>
<td> {row} </td>
<td> {val} </td>
</tr>""")
    print("</table>")
