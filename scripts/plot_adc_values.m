function [x] = plot_adc_values(path,num_channels=4,type='uint16')
f=fopen(path,'r');
x=fread(f,Inf,type);
fclose(f);
x=reshape(x,[num_channels length(x)/num_channels]);
if nargout() == 0
    plot(1:size(x,2),x);
end
