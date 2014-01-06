function [ allWarped, mask ] = PrepareImages( startIndex, endIndex, landmarkLocations, M, T )
    m2d = MtoMean2D(M);

    [ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, m2d);

    % Have to use matfile as can't fit all images in at once, so do it in 2
    % batches
    mpie = matfile('C:\dataset\mpie-frontal.mat');
    % An array of vectorised, warped images that is returned, the size of
    % an image is the number of 1's in the mask
%     allWarped = zeros(endIndex-startIndex+1,sum(mask(:)));
    downsize_mask = imresize(mask, 0.5);
    allWarped = zeros(endIndex-startIndex+1, size(downsize_mask, 1)*size(downsize_mask,2));

    images = mpie.allExamplesColour(startIndex:endIndex,:,:);
    
    allWarped(1:endIndex-startIndex+1,:) = WarpImages(images, landmarkLocations(startIndex:endIndex,:,:), T, triX, mask, alphas, betas, npix, xmin, ymin, 0.5);

end