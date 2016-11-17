figure(1);
x= plot_raw_sound('/tmp/tmp.XXXX','float64',2,1);
title('left channel');
figure(2);
y = plot_raw_sound('/tmp/tmp.XXXX','float64',2,2);
title('right channel');
