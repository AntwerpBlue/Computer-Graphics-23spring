%% Uniformly parameteriazation
imagefile='cathead.obj';

[obj.vertices,obj.faces,~,~]=readObj(imagefile);
%trisurf(obj.faces,obj.vertices(:,1),obj.vertices(:,2), obj.vertices(:,3) );
[obj.boundary,~]=findBoundary(obj.vertices,obj.faces);
obj.inner=setdiff((1:size(obj.vertices,1)),obj.boundary);

[uni_vertices,obj.adj_mat]=uni_para(obj);
figure;
drawmesh(obj.faces,uni_vertices,obj.boundary);
    
%% Bonus: weighted parameterization
wtd_vertices=wtd_para(obj);
figure;
drawmesh(obj.faces,wtd_vertices,obj.boundary);