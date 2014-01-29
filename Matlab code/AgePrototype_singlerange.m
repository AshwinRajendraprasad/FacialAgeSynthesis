function [ age_prototype ] = AgePrototype_singlerange( textures, sigma, mask )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

    avg_face = mean(textures);
    high_frequency = ones(1,size(textures,2));
    
    for i=1:size(textures,1)
        high_frequency = immultiply(high_frequency, (imdivide(textures(i,:),GaussianBlur(textures(i,:),sigma,mask)')));
    end
    % This is removing the high frequnecy components at the edges, however it doesn't do quite
    % enough, so also have the blanket maximum of 3 for high frequency
    mask_edge = edge(mask, 'sobel');
    se = strel('diamond', 2);
    mask_edge = imdilate(mask_edge, se);
    
    high_f_im = AddZerosToImage(mask, high_frequency);
    high_f_im(mask_edge) = 1;
    high_f_im = high_f_im(logical(mask));
    high_frequency = high_f_im(:);
    high_frequency(high_frequency>3) = 3;
    
    age_prototype = immultiply(high_frequency', avg_face);
end

