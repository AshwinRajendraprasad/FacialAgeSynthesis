function [ dist ] = PointToLine( x, y, x1, y1, x2, y2 )
%POINTTOLINE Calculates the distance from a point to a line segment

    % project the point onto a line and check if t is within [0; 1]
    % x = a + t*b
    
    b = [ x2 - x1; y2 - y1 ];
    
    a = [ x - x1; y - y1 ];
    
    t = dot(b,a) / (norm(b) ^ 2);

    if ( t < 0 )
       dist = norm(a); 
    elseif (t > 1)
        dist = norm( [x - x2; y - y2] );
    else
        
        n = [ y2 - y1; -( x2 - x1 ) ];
        dist = abs(dot(-a,n/norm(n)));
    end
    
end