function [ImageToDrawOn] = DrawTriangulation( ImageToDrawOn, DelaunayTri )
%DRAWOBJECT A helper function for drawing the outline of the shape

    XS = DelaunayTri.X;
    Tris = DelaunayTri.Triangulation;

    % Draw the outline of every triangle
    for j=1:size(DelaunayTri.Triangulation,1)
        for i=1:3
            x1 = XS(Tris(j,i),1);
            y1 = XS(Tris(j,i),2);
            
            x2 = XS(Tris(j,mod(i,3) + 1),1);
            y2 = XS(Tris(j,mod(i,3) + 1),2);            
            
            [~,~,ImageToDrawOn,~,~] = bresenham(ImageToDrawOn, [x1 y1; x2 y2],0);
        end
    end    

end

