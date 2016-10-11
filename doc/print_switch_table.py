N_MIDI_CHANS=16


for i in xrange(N_MIDI_CHANS):
    print "<tr>"
    print "<td>{0:d}</td>".format(i+1)
    print "{0:04b}".format(i).replace("1","<td>up</td>\n").replace("0","<td>middle/down</td>\n")
    print "</tr>"
