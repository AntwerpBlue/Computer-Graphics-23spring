function wtd_ver = wtd_para(obj)

n_pt=size(obj.vertices,1);
vec_x=zeros(nnz(obj.adj_mat(obj.inner,:)),1);
vec_y=zeros(nnz(obj.adj_mat(obj.inner,:)),1);
vec_val=zeros(nnz(obj.adj_mat(obj.inner,:)),1);

global x_bd;

idx=1;
for i=1:size(obj.vertices)
    if ismember(i,obj.inner)
%% get the subgraph of each inner_point
        adj_ver=find(obj.adj_mat(i,:));%get the adjacent vertices of p_i
        di=numel(adj_ver);
        sorted_adj_ver=zeros(1,di);
        sorted_adj_ver(1)=adj_ver(1);
        for j=2:di
            adj_verj=find(obj.adj_mat(sorted_adj_ver(j-1),:));
            joint_ele=intersect(adj_ver,adj_verj);
            left_ele=setdiff(joint_ele,sorted_adj_ver);
            if ~isempty(left_ele), sorted_adj_ver(j)=left_ele(1); end
        end
        side_points=obj.vertices(sorted_adj_ver,:);
        center_point=obj.vertices(i,:);
        r_i=sqrt(sum((side_points-obj.vertices(i,:)).^2,2));%module length
        product=dot((side_points-center_point)',(side_points([2:end 1],:)-center_point)');
        theta_i=acos(product'./(r_i.*r_i([2:end 1])));
        theta_i=theta_i*2*pi/sum(theta_i);%normalizition
        arg_i=tril(ones(di))*theta_i;
        
%% get the areal coordinates in each subgraph
        temp_arg=arg_i+pi;
        [larger,~]=find(temp_arg>2*pi);
        temp_arg(larger)=temp_arg(larger)-2*pi;
        rl_2=zeros(1,di);
        for j=1:di
            [larger,~]=find(arg_i>temp_arg(j));
            rl_2(j)=larger(1);%the number in sorted_adj_ver
        end
        rl_1=mod(rl_2-2,di)+1;
        Q=ones(3);
        x=[1 0 0]';
        val=zeros(di,1);
        for j=1:di
            Q((2:3),1)=r_i(j)*[sin(arg_i(j)) ; cos(arg_i(j))];
            Q((2:3),2)=r_i(rl_1(j))*[sin(arg_i(rl_1(j))) ; cos(arg_i(rl_1(j)))];
            Q((2:3),3)=r_i(rl_2(j))*[sin(arg_i(rl_2(j))) ; cos(arg_i(rl_2(j)))];
            Val=Q\x;
            val([j rl_1(j) rl_2(j)],1)=val([j rl_1(j) rl_2(j)],1)+Val;
        end
        vec_x(idx:(idx+di-1))=i*ones(di,1);
        vec_y(idx:(idx+di-1))=sorted_adj_ver;
        vec_val(idx:(idx+di-1))=val/di;
        idx=idx+di;
    end
end
A0=sparse(vec_x,vec_y,vec_val,n_pt,n_pt);
A_diag=diag(sum(A0'));
A=A_diag-A0;
P=A(obj.inner,obj.inner);
yb=A0(:,obj.boundary)*x_bd;
b=yb(obj.inner,:);
wtd_ver=zeros(n_pt,2);
wtd_ver(obj.inner,:)=P\b;
wtd_ver(obj.boundary,:)=x_bd;


end

