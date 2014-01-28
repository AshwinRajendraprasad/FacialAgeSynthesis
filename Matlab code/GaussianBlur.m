function [ image_blur ] = GaussianBlur( image, sigma, mask )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    image = AddZerosToImage(mask, image);

    G = fspecial('gaussian', [3 3], sigma);
    image_blur = imfilter(image, G);
    
    image_blur = image_blur(logical(mask));
    image_blur = image_blur(:);
end

