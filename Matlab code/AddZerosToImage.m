function [ image ] = AddZerosToImage( mask, nonzero_image )
%AddZerosToImage As the images are stored as only the masked pixels, this
%function takes a vectorised image stored in this way and returns the full
%size image in a matrix

    image = zeros(size(mask));
    image(logical(mask)) = nonzero_image;

end

