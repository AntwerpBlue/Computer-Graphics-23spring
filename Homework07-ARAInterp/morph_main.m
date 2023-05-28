[vertices_s,topo] = readObj('elephant_s');
vertices_t = readObj('elephant_t');
[boundary,~]=findBoundary(vertices_s,topo);

figure('position', [10 40 1210, 840]); subplot(131); drawmesh(topo, vertices_s);
subplot(133); drawmesh(topo, vertices_t);
subplot(132); h=drawmesh(topo, vertices_s);

boundary_triangles=find(sum(ismember(topo,boundary),2)>=2);

for time = 0:0.01:1
    %% placeholder: linear interpolating the vertex positions
    %z = (1-time)*vertices_s + time*vertices_t;
    
    %% TODO: finish the ARAP interpolation function
    z = ARAP_interp(vertices_s, vertices_t, topo, time);
    z=[z,zeros(size(vertices_s,1),1)];
    %% draw the result
    set(h,'vertices',z);
    drawnow; pause(0.01);
end



function h = drawmesh(t, x)
    h = trimesh(t, x(:,1), x(:,2), x(:,1), 'facecolor', 'interp', 'edgecolor', 'k');
    axis equal; axis off; view(2);
end