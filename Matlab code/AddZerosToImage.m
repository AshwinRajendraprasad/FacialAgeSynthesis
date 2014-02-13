function [ image ] = AddZerosToImage( mask, nonzero_image, numChannels )
%AddZerosToImage As the images are stored as only the masked pixels, this
%function takes a vectorised image stored in this way and returns the full
%size image in a matrix
    
    % Assume that there is only one channel (makes compatible with
    % grayscale code)
    if nargin<3
        numChannels = 1;
    end
    
    image = zeros(size(mask,1), size(mask,2), numChannels);
    if numChannels==1
        image(logical(mask)) = nonzero_image;
    else
        im = zeros(size(mask));
        for i=1:numChannels
            startInd = sum(mask(:))*(i-1)+1;
            endInd = sum(mask(:))*i;
            im(logical(mask)) = nonzero_image(startInd:endInd);
            image(:,:, i) = im;
        end
    end

end

