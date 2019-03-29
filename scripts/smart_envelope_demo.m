Amin = 0.001; % Minimum attack time
Amax = 0.5;   % Maximum attack time
Rmin = 0.001; % Minimum release time
Rmax = 0.5;   % Maximum release time
y=(0:0.01:1);
x=y.*2*pi;
A=(clip(sin(x-pi/4),[sin(-pi/4) sin(pi/4)])/sin(pi/4)+1)/2 ...
    *(Amax-Amin)+Amin;
R=(-clip(sin(x+pi/4),[sin(-pi/4) sin(pi/4)])/sin(pi/4)+1)/2 ...
    *(Rmax-Rmin)+Rmin;
plot(y,A,';Attack;',y,R,';Release;');
title('Envelope time at ENV knob positions')
ylabel('Ramp time as fraction of playback length as specified by LEN')
xlabel('Position of ENV knob (0 = counter-clockwise, 1 = clockwise)')
ylim([-0.1 0.6])
print -dpng '/tmp/env_ramp.png'
