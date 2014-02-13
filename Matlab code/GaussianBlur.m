function [ image_blur ] = GaussianBlur( image, sigma, mask, numChannels )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    image = AddZerosToImage(mask, image, numChannels);

    G = fspecial('gaussian', [3 3], sigma);
    image_blur = imfilter(image, G);
    
    if numChannels==1
        image_blur = image_blur(logical(mask));
        image_blur = image_blur(:);
    else
        texture = zeros(sum(mask(:)), 3);
        % Remove the values that are not covered by the mask, inefficient
        % to store them, have to do it per channel
        for j=1:numChannels
            im = image_blur(:,:,j);
            texture(:,j) = im(logical(mask));
        end
        % Vectorise the image
        image_blur = texture(:);
    end
            
end

