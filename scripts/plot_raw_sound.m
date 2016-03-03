function [x] = plot_raw_sound(path,
                              format="int16",
                              channels=2,
                              channel=1,
                              do_plot=1)
% PLOT_RAW_SOUND
% path = path to file
% format = the format of the samples (default="int16")
% channels = the number of channels, to know how many samples to skip
%   (default=2)
% channel = the channel that will be plotted. Indexing starts at 1.
%   (default=1)
% do_plot = whether or not to display the plot (can be used to just get the
%   values from a file). 0 is false, nonzero is true.
%   (default=1)
% 
% Returns
% The signal that it plotted.
f=fopen(path,"r");
x=fread(f,Inf,format);
x=x(channel:channels:end);
if (do_plot)
    plot(0:(length(x)-1),x);
end
