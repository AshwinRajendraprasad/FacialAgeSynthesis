clc;
clear;

I = checkerboard(10);
% J = im2double(zeros(size(I)));

controlPointsX = [10 10 30 50 50];
controlPointsY = [10 50 30 10 50];

oldPoints = [controlPointsX; controlPointsY]';

newPointsX = [1 10 48 50 50];
newPointsY = [1 50 25 1 50];

newPoints = [ newPointsX; newPointsY]';

tri = delaunay(newPointsX, newPointsY);

triplot(tri, newPointsX, newPointsY);


% find out the triangle locations for each pixel

textureSizeX = 50;
textureSizeY = 50;

[TriangleMap, WarpParamsMap] = AffinePreProcess(tri, newPoints, [textureSizeX textureSizeY]);

J = PieceWiseAffineWarp(I, [50 50], tri, oldPoints, TriangleMap, WarpParamsMap);

imshow(J);

% Transform = cp2tform(oldPoints, newPoints, 'piecewise linear');
% 
% % Need a way to draw the triangulation
% 
% triangleInd(:,:) = Transform.tdata.Triangles;
% 
% figure(1);
% 
% for i = 1:size(triangleInd, 1)    
%     xs = [newPointsX(triangleInd(i,:)) newPointsX(triangleInd(i,1))];
%     ys = [newPointsY(triangleInd(i,:)) newPointsY(triangleInd(i,1))];
%     line(xs, ys);    
%     
% end
% 
% figure(2);
% 
% J = imtransform(I, Transform,'XData', [200 600], 'YData', [100 500]);
% imshow(J);