function [ images ] = loadImages( mpie, number )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    images = mpie.allExamplesColour(1:number,:,:);

end

