clear
close all
clc

load("out.csv")

[Ntime, Nballs2] = size(out);
Nballs = Nballs2 / 2;
figure()
xmax = max(max(out(:,1:2:Nballs2)));
xmin = min(min(out(:,1:2:Nballs2)));

ymax = max(max(out(:,2:2:Nballs2)));
ymin = min(min(out(:,2:2:Nballs2)));
tic
for n=1:Ntime
    for i=1:Nballs
        plot(out(n,2*i-1), out(n,2*i),"Marker", "o");
        ylim([ymin, ymax])
        xlim([xmin, xmax])
        hold on
    end
    
    pause(1/25);
    delete(findobj("Marker", "o"))
end
toc