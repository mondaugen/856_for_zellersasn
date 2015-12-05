Amin = 0.001; % Minimum attack time
Amax = 2.0;   % Maximum attack time
Rmin = 0.001; % Minimum release time
Rmax = 2.0;   % Maximum release time
y=(0:0.01:1);
x=y.*2*pi;
A=(clip(sin(x-pi/4),[sin(-pi/4) sin(pi/4)])/sin(pi/4)+1)/2 ...
    *(Amax-Amin)+Amin;
R=(-clip(sin(x+pi/4),[sin(-pi/4) sin(pi/4)])/sin(pi/4)+1)/2 ...
    *(Rmax-Rmin)+Rmin;
plot(y*4,A,y*4,R);
