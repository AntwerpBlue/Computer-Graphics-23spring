function imret = blendImagePoisson(im1, im2, roi, targetPosition)
imret=im1;
% input: im1 (background), im2 (foreground), roi (in im2), targetPosition (in im1)

%% TODO: compute blended image
% imret = 255-im1;
Pointmin=floor(min(roi));
Pointmax=floor(max(roi));
Pointmin1=floor(min(targetPosition));
Pointmax1=floor(max(targetPosition));
im2copy=im2(Pointmin(2):Pointmax(2),Pointmin(1):Pointmax(1),:);
im1copy=im1(Pointmin1(2):Pointmax1(2),Pointmin1(1):Pointmax1(1),:);%find a sub-mat to cover the area
Map=zeros(Pointmax(2)-Pointmin(2)+1,Pointmax(1)-Pointmin(1)+1);
index=0;

for i=Pointmin(2):Pointmax(2)
    for j=Pointmin(1):Pointmax(1)
        [in,~]=inpolygon(i,j,roi(:,2),roi(:,1));
        if in==1
            Map(i-Pointmin(2)+1,j-Pointmin(1)+1)=index+1;%create the index-mat "Map" to find the location of the points
            index=index+1;
        end 
    end
end
A=-4*speye(index);
[m,n]=size(Map);

for k=1:3
    bg=double(im1copy(:,:,k));
    fg=double(im2copy(:,:,k));
    [dx1,dy1]=imgradientxy(fg/8,"sobel");
    [dx2,dy2]=imgradientxy(bg/8,"sobel");
    
    gn1 = dx1.^2 + dy1.^2;
    gn2 = dx2.^2 + dy2.^2;
    
    for i=1:m
        for j=1:n
            if Map(i,j)~=0&&gn1(i,j)<gn2(i,j)
                dx1(i,j)=dx2(i,j);
                dy1(i,j)=dy2(i,j);
            end
        end
    end
    
    [dxx,~]=imgradientxy(dx1/8,"sobel");
    [~,dyy]=imgradientxy(dy1/8,"sobel");
    
    lap=dxx+dyy;

    V=zeros(index,1,"double");
    for i=1:m
        for j=1:n
            neighbors=[i,j+1;i,j-1;i-1,j;i+1,j];
            if Map(i,j)~=0
                V(Map(i,j))=lap(i,j);
                if i==1||i==m||j==1||j==n
                    V(Map(i,j))=V(Map(i,j))-bg(i,j);
                else
                    for h=1:4
                        if Map(neighbors(h,1),neighbors(h,2))==0
                            V(Map(i,j))=V(Map(i,j))-bg(neighbors(h,1),neighbors(h,2));
                        else 
                            A(Map(i,j),Map(neighbors(h,1),neighbors(h,2)))=1;
                        end
                    end
                end
            end
        end
    end
    decA = decomposition(A, "auto");
    P=floor(decA\V);

    for i=1:m
        for j=1:n
            if Map(i,j)~=0
                imret(Pointmin1(2)+i-1,Pointmin1(1)+j-1,k)=P(Map(i,j));
            end
        end
    end
end

assignin("base","decA",decA);
assignin("base","Map",Map);
assignin("base","im2copy",im2copy);
assignin("base","index",index);
hpolys=evalin("base","hpolys");
hCallback=addlistener(hpolys(2),"MovingROI",@RealtimeCallback);
assignin("base","hCallback",hCallback);










