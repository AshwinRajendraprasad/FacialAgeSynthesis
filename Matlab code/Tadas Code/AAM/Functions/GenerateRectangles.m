function [rects] = GenerateRectangles(numRect, showRects)

%% Generating the rectangles

rects = zeros(numRect, 8);

for i=1:numRect,
   
    % get a length
    length = randi([1,20],1,1);
    
    % get a width
    width = randi([1,20],1,1);
    
    % get translation
    transX = randi([-10,10],1,1);
    transY = randi([-10,10],1,1);
    
    % get rotation
    rot = pi * rand(1,1);
    %rot = 0;
    
    xVals = [ -length/2 -length/2 length/2 length/2 ];
    yVals = [ -width/2 width/2 width/2 -width/2 ];
    
    xValsNew = xVals.*cos(rot) - yVals.*sin(rot);
    yValsNew = xVals.*sin(rot) + yVals.*cos(rot);
    
    xValsNew = xValsNew + transX;
    yValsNew = yValsNew + transY;
    
    rects(i,:) = [ xValsNew yValsNew ];
    
end

%% Visualisation
if(showRects),
    for i = 1:numRect,
       line([rects(i,1:4) rects(i,1)], [rects(i,5:8) rects(i,5)]);
    end
end    