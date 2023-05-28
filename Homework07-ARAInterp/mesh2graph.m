function adj = mesh2graph(topo)

n_f=size(topo,1);

adj_row=[];
adj_col=[];


for idx=1:n_f
    cnct=find(sum(ismember(topo,topo(1,:)),2)>=2);
    cnct(cnct==idx)=[];
    adj_row=[adj_row,idx*ones(1,size(cnct,1))];
    adj_col=[adj_col,cnct'];
end

adj=sparse(adj_row,adj_col,ones(1,size(adj_row,2)),n_f,n_f);

end

