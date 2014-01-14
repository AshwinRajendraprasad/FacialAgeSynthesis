function [ OutputImage ] = PieceWiseAffineWarp( SourceImage, OutputSize, Triangulation, SourceData, TriangleMap, WarpParamMap )
%PIECEWISEAFFINEWARP Summary of this function goes here
%   Detailed explanation goes here
    OutputImage = zeros([OutputSize 3]);
    
    % Build a system that maps the 
    % traverse over every pixel in the image        
    
    for x=1:OutputSize(1)
       
        for y=1:OutputSize(2)
            
            % get the corresponding triangle
            triInd = TriangleMap(x,y);
            
            if(triInd == 0)
               continue; 
            end
            
            % get the parameters
            WarpParams = squeeze(WarpParamMap(x,y,1:3));

            ind = Triangulation(triInd,1:3);            
            
            xSource = WarpParams(1:3)' * SourceData(ind, 1);
            ySource = WarpParams(1:3)' * SourceData(ind, 2);
            
           try
            OutputImage(y,x,:) = SourceImage.I(round(ySource), round(xSource),:);
           catch ME1
                fprintf('Cannot do x:%f,y:%f,xSource:%f,ySource:%f\n', x,y,xSource,ySource);
           end
        end
        
    end

    OutputImage = im2double(OutputImage);
    imshow(OutputImage);
    
end

