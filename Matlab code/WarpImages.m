function [warpeds] = WarpImages( images, landmarkLocations,  T, triX, mask, alphas, betas,npix,xmin, ymin)
    imSize = size(images);
    warpeds = zeros(imSize(1), 151, 148);
    
    parfor i=1:imSize(1)
        image = images(i,:,:);
        imageLandmarks = squeeze(landmarkLocations(i,:,:));
        warpIm = Crop(squeeze(image), imageLandmarks, T, triX, mask, alphas, betas,npix,xmin, ymin);
        warpeds(i,:,:) = warpIm;
    end
end