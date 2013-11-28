function [ mapX, mapY ] = WarpRegion( xmin, ymin, mask, triX, coeffs )
%WARPREGION Summary of this function goes here
%   Detailed explanation goes here

    [h, w] = size(mask);

    k = -1;
    
    mapX = zeros(size(mask));
    mapY = zeros(size(mask));
 
    for y=1:h
        yi = y + ymin - 1;

        for x=1:w
            xi = x + xmin - 1;
            if(mask(y,x) == 0)

                mapX(y,x) = -1;
                mapY(y,x) = -1;
            else
                j = triX(y,x); 
                if(j ~= k)				
                    a = coeffs(j+1,:); 
                    k = j;
                end  	

                xo = a(1);
                xo = xo + a(2) * xi;
                mapX(y,x) = xo + a(3) * yi;

                yo = a(4);
                yo = yo + a(5) * xi;
                mapY(y,x) = yo + a(6) * yi;
            end
        end
    end
end

