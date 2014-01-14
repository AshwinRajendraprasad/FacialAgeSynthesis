function [ Texture ] = Vec2TexCol( Vector, mask, size, numberOfBands, bandSize )
%VEC2TEXCOL Summary of this function goes here
%   Detailed explanation goes here

    Texture = zeros([size, numberOfBands]);
    
    Tex = zeros(size);
    
    for i = 1:numberOfBands
        Tex(mask) = Vector(bandSize * (i - 1) + 1: bandSize * i);
        Texture(:,:,i) = Tex;
    end

end

