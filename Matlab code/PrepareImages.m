function [ allWarped ] = PrepareImages( startIndex, endIndex, landmarkLocations, M, T )
    m2d = MtoMean2D(M);

    [ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, m2d);

    % Have to use matfile as can't fit all images in at once, so do it in 2
    % batches
    mpie = matfile('C:\dataset\mpie-frontal.mat');
    % An array of vectorised, warped images that is returned, the size of
    % an image is the same as the size of the mask
    allWarped = zeros(endIndex-startIndex+1,size(mask,1)*size(mask,2));

    images = mpie.allExamplesColour(startIndex:endIndex,:,:);
    allWarped(1:endIndex-startIndex+1,:) = WarpImages(images, landmarkLocations(startIndex:endIndex,:,:), T, triX, mask, alphas, betas, npix, xmin, ymin);

end

% for i=1:numImages
%     imshow(squeeze(allWarped(i,:,:))/255);
%     waitforbuttonpress;
% end

% load('tri66.mat');
% load('pdm_aligned.mat', 'M');
% load('C:\dataset\mpie-frontal.mat', 'landmarkLocations');