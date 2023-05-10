clear
close all
clc
figure("Visible","off")

wb = waitbar(0, 'Starting');

load("out.txt")

[Ntime, Nballs2] = size(out);
Nballs = Nballs2 / 2;

xmax = 200;
xmin = 0;

ymax = 200;
ymin = 0;
F(Ntime) = struct('cdata',[],'colormap',[]);
I = imread('sapce.jpg'); 
tic
for n=1:Ntime
     
    viscircles(out(n,1:2), 1, "LineWidth",.1, "Color","green");
    hold on
    viscircles([out(n,3:2:end)', out(n,4:2:end)'],1, "LineWidth",.1);
    axis equal
    ylim([ymin, ymax])
    xlim([xmin, xmax])
    hold on
    
    %set(gca,'Color','k')
    h = image(xlim,ylim,I); 
    uistack(h,'bottom')
    
    waitbar(n/Ntime, wb, sprintf('Progress: %d %%', floor(n/Ntime*100)));
    children = get(gca, 'children');
    F(n) = getframe(gca);
    delete(children);
end
toc
close(wb)

vw = VideoWriter("balls");
vw.FrameRate = 50;
vw.Quality = 100;
open(vw);
writeVideo(vw, F);
close(vw);
