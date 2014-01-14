function [textures] = WarpImages( images, landmarkLocations,  T, triX, mask, alphas, betas,npix,xmin, ymin)

    textures = zeros(size(images,1), sum(mask(:)));
    numImages = size(images,1);
    
    for i=1:numImages
        image = images(i,:,:);
        imageLandmarks = squeeze(landmarkLocations(i,:,:));
        texture = Crop(squeeze(image), imageLandmarks, T, triX, mask, alphas, betas,npix,xmin, ymin);
        % Vectorise the image and store it in the list of warped images and
        % remove the zero pixels that don't need to be stored
        texture_vec = texture(:);
        textures(i,:) = texture_vec(logical(mask(:)));
    end
end