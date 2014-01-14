function [ TriangleMap, WarpParamsMap ] = AffinePreProcess( Triangulation, ControlPoints, Size )
%AFFINEPREPROCESS Summary of this function goes here
%   Detailed explanation goes here

    TriangleMap = zeros(Size);
    WarpParamsMap = zeros([Size 3]);
    
    % go through the triangles (min max) and get all of the control warp
    % and trangles map for the pixels in those triangles
    
    for tri = 1:size(Triangulation,1)
        
        % get the bounding box first
        v1 = ControlPoints(Triangulation(tri,1),:);
        v2 = ControlPoints(Triangulation(tri,2),:);
        v3 = ControlPoints(Triangulation(tri,3),:);

        % check against the bounding box first
        minX = round(min([v1(1) v2(1) v3(1)]));
        maxX = round(max([v1(1) v2(1) v3(1)]));

        minY = round(min([v1(2) v2(2) v3(2)]));
        maxY = round(max([v1(2) v2(2) v3(2)]));
        
        if(minX < 1), minX = 1; end;
        if(minY < 1), minY = 1; end;
        if(maxX > Size(1)), maxX = Size(1); end;
        if(maxY > Size(2)), maxX = Size(2); end;
        
        for x = minX:maxX
            
            for y = minY:maxY
                if(PointInTriangle([x y 0], [v1 0], [v2 0], [v3 0]))
                    
                    TriangleMap(x,y) = tri;
                    
                    X = [x y 1]';
                    
                    A = [ v1 1; v2 1; v3 1 ]';                                                            
                    WarpParamsMap(x,y,:) = A\X;
                    
                    continue;
                end            
            end
                
        end
        
    end
    
%     for x = 1:Size(1)
% 
%         for y = 1:Size(2)
%             
%             for tri = 1:size(Triangulation,1)
%                
%                 v1 = ControlPoints(Triangulation(tri,1),:);
%                 v2 = ControlPoints(Triangulation(tri,2),:);
%                 v3 = ControlPoints(Triangulation(tri,3),:);
%                 
%                 % check against the bounding box first
%                 minX = min([v1(1) v2(1) v3(1)]);
%                 maxX = max([v1(1) v2(1) v3(1)]);
%                 
%                 minY = min([v1(2) v2(2) v3(2)]);
%                 maxY = max([v1(2) v2(2) v3(2)]);
%                 
%                 if(x < minX || x > maxX || y < minY || y > maxY) 
%                     continue;                    
%                 end
%                 if(PointInTriangle([x y 0], [v1 0], [v2 0], [v3 0]))
%                     TriangleMap(x,y) = tri;
%                     
%                     X = [x y 1]';
%                     
%                     A = [ v1 1; v2 1; v3 1 ]';                                                            
%                     WarpParamsMap(x,y,:) = A\X;
%                     
%                     continue;
%                 end
%             end
%             
%         end
%         
%     end

end

function inTriangle = PointInTriangle(point, v1, v2, v3)

    inTriangle = SameSide(point, v1,v2,v3) && SameSide(point, v2,v1,v3) && SameSide(point, v3,v1,v2);

end

function sameSide = SameSide(toTest,v1,v2,v3)

    cross1 = cross( v3 - v2, toTest - v2);
    cross2 = cross( v3 - v2, v1 - v2);
    
    sameSide = (cross1 * cross2') >= 0;
    
end
