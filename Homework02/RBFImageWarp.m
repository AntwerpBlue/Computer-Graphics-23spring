function im2 = RBFImageWarp(im, psrc, pdst)

% input: im, psrc, pdst


%% basic image manipulations
% get image (matrix) size
[h, w, dim] = size(im);

im2=zeros(h,w,dim,"uint8");

%% use loop to negate image
% for i=1:h
%     for j=1:w
%         for k=1:dim
%             im2(i, j, k) = 255 - im(i, j, k);
%         end
%     end
% end
% 

%% TODO: compute warpped image
N=size(psrc,1);
P=zeros(N);
d=1e6;
delta=2000;

% for i=1:1:N
%     for j=1:1:N
%         p=psrc(i,:);
%         q=pdst(j,:);
%         %P(i,j)=1/(norm(p-q)^2+d);
%         P(i,j)=exp(-norm(p-q)^2/delta);%using Gauss-fun
%     end
% end
% 
% %A=P\pdst;
% A=P\pdst;
% A=A-psrc;
% for i=1:h
%     for j=1:w
%         for k=1:dim
%             x=[j,i];
%             fx = zeros(1,2);
%             for m = 1:N
%                 %fx = fx+A(m,:)/(norm(x-psrc(m,:))^2+d);
%                 fx=fx+A(m,:)*exp(-norm(x-psrc(m,:))^2/delta);%using Gauss-fun
%             end
%             fx=fx+x;
%             if fx(1,1)>0&&ceil(fx(1,1))<=h&&fx(1,2)>0&&ceil(fx(1,2))<=w
%                 im2(ceil(fx(1,2)),ceil(fx(1,1)),k)=im(i,j,k);
%             else
%             end
%         end
%     end
% end

%% try to use convolution to solve the black webs
% % Create Gauss filter
% kernel_size = 5;
% sigma = 2;
% gauss_kernel = fspecial('gaussian', [kernel_size kernel_size], sigma);
% 
% % convolution
% im2(:,:,1) = conv2(im2(:,:,1), gauss_kernel, 'same');
% im2(:,:,2) = conv2(im2(:,:,2), gauss_kernel, 'same');
% im2(:,:,3) = conv2(im2(:,:,3), gauss_kernel, 'same');

%% consider to inverse the transeformation
% for i=1:1:N
%     for j=1:1:N
%         p=psrc(j,:);
%         q=pdst(i,:);
%         P(i,j)=1/(norm(p-q)^2+d);
%     end
% end
% 
% %A=P\pdst;
% A=P\(psrc-pdst);
% 
% 
% for i=1:h
%     for j=1:w
%         for k=1:dim
%             %x=[j,i];
%             sum=0;
%             x=[j,i];
%             for t=1:N
%                 sum=sum+A(t,:)/(norm(x-psrc(t,:))^2+d);
%             end
%             F=@(x) sum+x-[i,j];
%             solution=fzero(F,[1,1]);
%             if solution(1,1)>0&&ceil(solution(1,1))<=h&&solution(1,2)>0&&ceil(solution(1,2))<=w
%                 im2(i,j,k)=im(ceil(solution(1,2)),ceil(solution(1,1)),k);
%             end
%         end
%     end
% end        

%% using a formal inverse-trans
for i=1:1:N
    for j=1:1:N
        p=psrc(i,:);
        q=pdst(j,:);
        P(j,i)=exp(-norm(p-q)^2/delta);
    end
end
A=P\(psrc-pdst);
for i=1:h
    for j=1:w
        for k=1:dim
            x=[j,i];
            fx = zeros(1,2);
            for m = 1:N
                %fx = fx+A(m,:)/(norm(x-psrc(m,:))^2+d);%using power-fun
                fx=fx+A(m,:)*exp(-norm(x-psrc(m,:))^2/delta);%using Gauss-fun
            end
            fx=fx+x;
            if fx(1,1)>0&&ceil(fx(1,1))<=w&&fx(1,2)>0&&ceil(fx(1,2))<=h
                im2(i,j,k)=im(ceil(fx(1,2)),ceil(fx(1,1)),k);
            else
            end
        end
    end
end
