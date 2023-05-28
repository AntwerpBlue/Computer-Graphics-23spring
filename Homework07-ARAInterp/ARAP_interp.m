function z = ARAP_interp(ver_s,ver_t,topo,t)

n_f=size(topo,1);
n_pt=size(ver_s,1);
D=[1,0,-1;0,1,-1];
A=[];

P_row=zeros(1,6*n_f);
P_value=zeros(1,6*n_f);
P_col=zeros(1,6*n_f);

W_value=zeros(1,n_f);

adj=mesh2graph(topo);

for idx=1:n_f
    p_t=ver_s(topo(idx,:),:);p_t=p_t(:,(1:2));
    q_t=ver_t(topo(idx,:),:);q_t=q_t(:,(1:2));
    p_t_star=(D*p_t)\D;
    A_i=p_t_star*q_t;
    [R_a,Sigma,R_b]=svd(A_i);
    R_b=R_b';%note that svd returns v'
    R_r=R_a*R_b;%rotation matrix
    S=R_b'*Sigma*R_b;%symmetric matrix
    rot_angle=angle(R_r(1)+1i*R_r(2));
    R_t=[cos(rot_angle*t),-sin(rot_angle*t);sin(rot_angle*t),cos(rot_angle*t)];
    A_t=R_t*((1-t)*eye(2)+t*S);
    A=[A;A_t];
    side=D*p_t;
    W_value(1,idx)=det(side)/2;
    
    P_row((6*(idx-1)+1):(6*idx))=repmat([2*idx-1,2*idx],1,3);
    P_col((6*(idx-1)+1):(6*idx))=reshape(repmat(topo(idx,:),2,1),1,6);
    P_value((6*(idx-1)+1):(6*idx))=reshape(p_t_star,1,6);
    
end

P=sparse(P_row,P_col,P_value,2*n_f,n_pt);

W_value=reshape(repmat(W_value,2,1),1,2*n_f);
W=sparse([1:2*n_f],[1:2*n_f],W_value,2*n_f,2*n_f);

C=[1,zeros(1,n_pt-1)];
D=ver_s(1,(1:2));
LHS=[P'*W*P,C';C,0];
RHS=[P'*W*A;D];
V=LHS\RHS;

z=V((1:n_pt),:);
end

