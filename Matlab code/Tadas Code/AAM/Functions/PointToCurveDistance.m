function [ dist ] = PointToCurveDistance( Shape1, Shape2, paths, types )
%POINTTOCURVEDISTANCE Summary of this function goes here
%   Detailed explanation goes here

    dists = [];

    for j=1:max(paths) + 1
        
        toConnectFirst = find(paths == j - 1, 1, 'first');
        toConnectLast = find(paths == j - 1, 1, 'last');

        path1X = Shape1(toConnectFirst : toConnectLast,1);
        path1Y = Shape1(toConnectFirst : toConnectLast,2);

        path2X = Shape2(toConnectFirst : toConnectLast,1);
        path2Y = Shape2(toConnectFirst : toConnectLast,2);

        cyclic = types(toConnectFirst) == 0;
        
        dists = [dists CurveToCurveDist(path1X, path1Y, path2X, path2Y, cyclic)'];
        
    end

	dist = mean(dists);

end

% Calculate the distance from curve to curve
function [ dists ] = CurveToCurveDist(xs1, ys1, xs2, ys2, cyclic)

    dists = zeros(numel(xs1),1);

    for i = 1:numel(xs1)
       dists(i) = PointToCurve(xs1(i), ys1(i), xs2, ys2, cyclic);  
    end

end

% Calculate the distance of the given point to a curve, depending on the
% type of curve
function [ dist ] = PointToCurve( x, y, xs, ys, cyclic )

    % find the point on the curve closest to the given point
    dists = sqrt((xs - x).^2 + (ys - y).^2);
    [dist, minInd] = min(dists);

    pathLength = numel(xs);
    
    indBefore = mod(minInd - 2, pathLength) + 1; 
    indAfter = mod(minInd, pathLength) + 1;
        
    % calculate the smallest distance from the two closest line segments
    if(cyclic)
       
        % if cyclic we can compare both lines, if not cyclic have to be more careful 
        dist = min([dist, PointToLine(x,y,xs(indBefore),ys(indBefore), xs(minInd), ys(minInd)),...
                     PointToLine(x,y,xs(minInd), ys(minInd), xs(indAfter),ys(indAfter))]);
        
    else
        if(minInd == 1)
            dist = min([dist, PointToLine(x,y,xs(minInd), ys(minInd), xs(indAfter),ys(indAfter))]);           
        elseif(minInd == pathLength)
            dist = min([dist, PointToLine(x,y,xs(indBefore),ys(indBefore), xs(minInd), ys(minInd))]);           
        else
        dist = min([dist, PointToLine(x,y,xs(indBefore),ys(indBefore), xs(minInd), ys(minInd)),...
                     PointToLine(x,y,xs(minInd), ys(minInd), xs(indAfter),ys(indAfter))]);            
        end
    end

end

