function [warpeds] = WarpImages( images, landmarkLocations,  T, triX, mask, alphas, betas,npix,xmin, ymin, downsize_factor)
    % Only store the pixels that are in the mask, so the size is the
    % number of 1's in the mask
%     warpeds = zeros(size(images,1), sum(mask(:)));
    downsize_mask = imresize(mask, downsize_factor);
    warpeds = zeros(size(images,1), size(downsize_mask,1)*size(downsize_mask,2));
    numImages = size(images,1);
    
    for i=1:numImages
        image = images(i,:,:);
        imageLandmarks = squeeze(landmarkLocations(i,:,:));
        warpIm = Crop(squeeze(image), imageLandmarks, T, triX, mask, alphas, betas,npix,xmin, ymin);
        warpIm = imresize(warpIm, downsize_factor);
        % Vectorise the image and store it in the list of warped images and
        % remove the zero pixels that don't need to be stored
        warpeds(i,:) = warpIm(:);
    end
end