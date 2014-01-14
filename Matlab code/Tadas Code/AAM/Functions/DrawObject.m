function [Image] = DrawObject( xVals, yVals, path, type, Image )
%DRAWOBJECT A helper function for drawing the outline of the shape
%   input x and y coordinates of the shape, together with connection
%   details and types of connections    
    
    for j=1:max(path) + 1
        % connecting the lines based on the path, assume they're
        % ordered    
        toConnectFirst = find(path == j - 1, 1, 'first');
        toConnectLast = find(path == j - 1, 1, 'last');

        connectX = xVals(toConnectFirst : toConnectLast);
        connectY = yVals(toConnectFirst : toConnectLast);

        if(type(toConnectFirst) == 4)
            for i = 1:numel(connectX)-1
                [~,~,Image,~,~] = bresenham(Image, [connectX(i) connectY(i); connectX(i+1) connectY(i+1)],0);
            end                          
        else
            for i = 1:numel(connectX)-1
                [~,~,Image,~,~] = bresenham(Image, [connectX(i) connectY(i); connectX(i+1) connectY(i+1)],0);
            end                          
            [~,~,Image,~,~] = bresenham(Image, [connectX(1) connectY(1); connectX(end) connectY(end)],0);
        end       

    end
    
end

