function [ avg_error ] = AppearanceModelAvgErr( AppearanceModel, testTextures )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    avg_error = zeros(size(testTextures,1),1);
    
    for i=1:size(testTextures,1)
        b = FindModelParameters(AppearanceModel, testTextures(i,:));
        texture = AppearanceParams2Texture(b, AppearanceModel);
        
        avg_error(i) = rms(texture' - testTextures(i,:));
    end
end

