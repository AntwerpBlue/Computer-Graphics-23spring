function [uni_ver,adj_mat] = uni_para(obj)
global x_bd;
n_pt=size(obj.vertices,1);
n_bd=numel(obj.boundary);

A1=sparse(obj.faces,obj.faces(:,[2,3,1]),1,n_pt,n_pt);
A2=sparse(obj.faces,obj.faces(:,[3,1,2]),1,n_pt,n_pt);
A0 = A1|A2;
adj_mat=A0;
A_diag=diag(sum(A0));
A=A_diag-A0;

cx_bd=exp(2i*pi*(0:n_bd-1)'/n_bd);
x_bd=[real(cx_bd),imag(cx_bd)];%the coordinates of boundary points
yb=A0(:,obj.boundary)*x_bd;
P=A(obj.inner,obj.inner);
b=yb(obj.inner,:);

uni_ver=zeros(n_pt,2);% stores the 2-D coordinates of all the vertices
uni_ver(obj.inner,:)=P\b;
uni_ver(obj.boundary,:)=x_bd;


end

