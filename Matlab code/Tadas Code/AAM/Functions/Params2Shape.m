function [ Shape ] = Params2Shape( PrincipalComponents, Bs, MeanShape, T )
%PARAMS2SHAPE Summary of this function goes here
%   Detailed explanation goes here

    %% Creating the shape from parameters
    ShapeOffset = PrincipalComponents * Bs;
    ShapeOffset = [ShapeOffset(1:end/2) ShapeOffset(end/2+1:end)];

    Shape = MeanShape + ShapeOffset;
        
    % Transform the shape to image plane (scale, rotate, translate), use an affine transform matrix            
    Ta = [1 + T(1), -T(2), T(3); T(2), 1 + T(1), T(4); 0, 0, 1];
    
    Shape = [Shape ones(size(Shape,1),1)]';
    
    Shape = Ta * Shape;
    
    Shape = Shape';
    Shape = Shape(:,1:2);
    
end

