numImages = 1000;

load('tri66.mat');
load('pdm_aligned.mat', 'M');
load('C:\dataset\mpie-frontal.mat', 'landmarkLocations');
m2d = MtoMean2D(M);

[ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, m2d);

% Have to use matfile as can't fit all images in at once, so do it in 2
% batches
mpie = matfile('C:\dataset\mpie-frontal.mat');
% 151x148 because this is the size of the mask, so the warped images are
% this size
allWarped = zeros(numImages,151,148);

images = mpie.allExamplesColour(1:ceil(numImages/2),:,:);
warpedImages = WarpImages(images, landmarkLocations(1:ceil(numImages/2),:,:), T, triX, mask, alphas, betas, npix, xmin, ymin);
allWarped(1:ceil(numImages/2),:,:) = warpedImages;

images = mpie.allExamplesColour(ceil(numImages/2)+1:numImages,:,:);
warpedImages = WarpImages(images, landmarkLocations(ceil(numImages/2)+1:numImages,:,:), T, triX, mask, alphas, betas, npix, xmin, ymin);
allWarped(ceil(numImages/2)+1:numImages,:,:) = warpedImages;

clearvars warpedImages alphas betas triX mask xmin ymin npix images

% for i=1:numImages
%     imshow(squeeze(allWarped(i,:,:))/255);
%     waitforbuttonpress;
% end