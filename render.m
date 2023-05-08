clear
close all
clc
figure("Visible","off")
wb = waitbar(0, 'Starting');

load("out.txt")

[Ntime, Nballs2] = size(out);
Nballs = Nballs2 / 2;

xmax = 100;
xmin = 0;

ymax = 100;
ymin = 0;

Ntime = 200;

F(Ntime) = struct('cdata',[],'colormap',[]);

tic
for n=1:Ntime
     
    for i=1:Nballs
        %plot(out(n,2*i-1), out(n,2*i),"Marker","o");
        
        circle(out(n,2*i-1), out(n,2*i),1);
        axis equal
        ylim([ymin, ymax])
        xlim([xmin, xmax])
        hold on
    end
    
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

function h = circle(x,y,r)
    hold on
    th = 0:pi/50:2*pi;
    xunit = r * cos(th) + x;
    yunit = r * sin(th) + y;
    h = plot(xunit, yunit, "Color","black");
    hold off
end