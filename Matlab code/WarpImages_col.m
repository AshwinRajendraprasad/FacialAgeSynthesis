function [textures] = WarpImages_col( images, landmarkLocations,  T, triX, mask, alphas, betas,npix,xmin, ymin)
 
    numImages = size(images,1);
    textures = zeros(numImages, sum(mask(:))*3);
    
    for i=1:numImages
        image = squeeze(images(i,:,:,:));
        imageLandmarks = squeeze(landmarkLocations(i,:,:));
        % Crop each channel one at a time
        texture = zeros(sum(mask(:)), 3);
        for j=1:3
            tex = Crop(image(:,:,j), imageLandmarks, T, triX, mask, alphas, betas,npix,xmin, ymin);
            % Remove the values that are not covered by the mask, inefficient
            % to store them
            texture(:,j) = tex(logical(mask));
        end
        % Vectorise the image and store it in the list of warped images
        textures(i,:) = texture(:);
    end
end