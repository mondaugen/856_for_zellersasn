from math import *

canvas_height=10
canvas_width=10

case_width=5
case_height=4
n_knobs=8
knob_space=0.5

draw_knob_string=(
'''\
<circle cx="{0}in" cy="{1}in" r="0.25in" stroke="black" stroke-width="1" \
fill="none" />
<line x1="{0}in" y1="{1}in" x2="{2}in" y2="{3}in" stroke="black" stroke-width="2"/>\
''')

draw_switch_string=(
'''\
<circle cx="{0}in" cy="{1}in" r="0.12in" \
stroke="black" stroke-width="1" fill="none" />
<circle cx="{0}in" cy="{1}in" r="0.05in" \
stroke="black" stroke-width="1" fill="none" />\
''')

svg_open_tag=(
'''\
<svg viewBox="0 0 {2} {3}" width="{0}in" height="{1}in" \
xmlns="http://www.w3.org/2000/svg">\
''')

def draw_knob(x,y):
    return draw_knob_string.format(x,y,x-0.25*cos(2.*pi*45/360.),y+0.25*sin(2.*pi*45/360.))

def draw_switch(x,y):
    return draw_switch_string.format(x,y)

#print '<html>'
print svg_open_tag.format(
        canvas_height,
        canvas_width,
        1*canvas_height*96,
        1*canvas_width*96)


for i in xrange(n_knobs/2):
    print draw_knob(case_width*(2*i+1)/9.,case_height/7.)
    print draw_knob(case_width*(2*i+2)/9.,case_height/7.*2)
    print draw_switch(case_width*(2*i+1)/9.,case_height/7.*2)
    print draw_switch(case_width*(2*i+2)/9.,case_height/7.)

print '<text x="{0}in" y="{0}in">HI GUYSS</text>'.format(
        canvas_width*(1/9.),canvas_height*3/7.)
print '</svg>'
#print '</html>'
