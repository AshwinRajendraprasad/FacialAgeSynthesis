function [ image ] = AddZerosToImage( mask, nonzero_image )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    image = zeros(1,size(mask(:),1));
    image_pixel = 1;
    for i=1:size(mask(:),1)
        if mask(i) == 1
            image(i) = nonzero_image(image_pixel);
            image_pixel = image_pixel+1;
        end
    end

end

