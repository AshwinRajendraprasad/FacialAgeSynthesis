function [warpeds] = WarpImages( images, landmarkLocations,  T, triX, mask, alphas, betas,npix,xmin, ymin)
    % The size is the number of images, by the number of pixels in a warped
    % image, as I will store and use the images in a vectorised form
    warpeds = zeros(size(images,1), size(mask,1)*size(mask,2));
    
    parfor i=1:size(images,1)
        image = images(i,:,:);
        imageLandmarks = squeeze(landmarkLocations(i,:,:));
        warpIm = Crop(squeeze(image), imageLandmarks, T, triX, mask, alphas, betas,npix,xmin, ymin);
        % Vectorise the image and store it in the list of warped images
        warpeds(i,:) = warpIm(:);
    end
end