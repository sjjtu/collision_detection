clear
close all
clc

load("out.txt")

[Ntime, Nballs2] = size(out);
Nballs = Nballs2 / 2;
figure()
xmax = max(max(out(:,1:2:Nballs2)));
xmin = min(min(out(:,1:2:Nballs2)));

ymax = max(max(out(:,2:2:Nballs2)));
ymin = min(min(out(:,2:2:Nballs2)));
tic
for n=1:Ntime
    n
    for i=1:Nballs
        %plot(out(n,2*i-1), out(n,2*i),"Marker","o");
        
        circle(out(n,2*i-1), out(n,2*i),1);
        axis equal
        ylim([ymin, ymax])
        xlim([xmin, xmax])
        hold on
    end
    
    pause(1/25);
    children = get(gca, 'children');
    delete(children);
end
toc


function h = circle(x,y,r)
    hold on
    th = 0:pi/50:2*pi;
    xunit = r * cos(th) + x;
    yunit = r * sin(th) + y;
    h = plot(xunit, yunit, "Color","black");
    hold off
end