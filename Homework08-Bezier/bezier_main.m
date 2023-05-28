figure; 
t = 0:0.001:1;

%%
h = drawpolyline;
hold on;
hcurve = plot(bezier(h.Position, t), 'g', 'linewidth', 2);
h.addlistener('MovingROI', @(h, evt) bezier(evt.CurrentPosition, t, hcurve));
%evt is an ROI Moving Event

%%
function p = bezier(p, t, h)
    p = p*[1;1i];
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
