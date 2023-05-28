figure; 
t = 0:0.001:1;

%%
h = drawpolyline;
hold on;
con_points=getconstrain(h.Position);
curve=getcurve(con_points,t);
hcurve=plot(curve,'r','linewidth',2);
set(h,'Position',[real(con_points),imag(con_points)]);
% h.addlistener('MovingROI', @(h,evt)getcurve(evt.CurrentPosition,t,hcurve));
%evt is an ROI Moving Event

%%

function con_points=getconstrain(p)
    np=size(p,1);
    p1=zeros(np-1,1);p2=zeros(np-1,1);%constrain points
    p=p*[1;1i];
    midp=zeros(np-1,1);midpp=zeros(np-2,1);%midp be mid of vertexs; midpp be mid of midp
    midp(1)=(p(1)+p(2))/2;
    for idx=2:np-1
        midp(idx)=(p(idx)+p(idx+1))/2;
        midpp(idx-1)=(midp(idx-1)+midp(idx))/2;
        p1(idx-1)=midp(idx-1)+(p(idx)-midpp(idx-1));
        p2(idx)=midp(idx)+(p(idx)-midpp(idx-1));
    end
    p2(1)=(p(1)+p1(1))/2;
    p1(np-1)=(p(np)+p2(np-1))/2;
    con_points=zeros(3*np-2,1);
    con_points(1)=p(1);con_points(2)=p2(1);
    for idx=1:np-2
        con_points(3*idx)=p1(idx);
        con_points(3*idx+1)=p(idx+1);
        con_points(3*idx+2)=p2(idx+1);
    end
    con_points(3*np-3)=p1(np-1);
    con_points(3*np-2)=p(np);
end

function curve=getcurve(con_points,t)
    curve=[];
    np=(size(con_points,1)+2)/3;
    for idx=1:np-1
        curve=[curve;bezier(con_points(3*idx-2:3*idx+1),t)];
    end
end

function p = bezier(p, t, h)
%     p = p*[1;1i];
    B=Bernstein(size(p,1)-1,t);
    p=B'*p;
    if nargin>2
        set(h, 'xdata', real(p), 'ydata', imag(p)); 
    end
end

function B=Bernstein(n,t)
    coe=diag(flipud(pascal(n+1)));
    num=(t'.^(0:n)).*((1-t').^(n:-1:0));
    B=coe.*num';
end