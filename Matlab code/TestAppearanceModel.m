function [ diff ] = TestAppearanceModel( AppearanceModel, test_textures )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    
    numImages = size(test_textures, 1);
    diff = 0;
    
    for i=1:numImages
        b = FindModelParameters(AppearanceModel, test_textures(i,:));
        texture = AppearanceParams2Texture(b, AppearanceModel);
        
        diff = diff + sum(imabsdiff(texture', test_textures(i,:)));
    end
    
    % divide the difference by number of test images to make it possible to
    % compare
    diff = diff/size(test_textures,1);

end

