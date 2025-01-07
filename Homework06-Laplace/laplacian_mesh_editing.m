nv = size(x, 1);

q=quatnormalize(qu_rotation');
R=quat2dcm(q);%convert quat to rotation matrix

y = x;
y(P2PVtxIds,:) = p2pDsts;
nc=numel(P2PVtxIds);
C_loc=sparse([1:nc],P2PVtxIds,1,nc,nv);
A=[L;C_loc];
delta=L*x;%using the uniformly weighted coordinate
C_cor=y(P2PVtxIds,:);
b=[delta;C_cor];
b(P2PVtxIds,:)=delta(P2PVtxIds,:)*R;
z=(A'*A)\(A'*b);
y = z((1:nv),:);
