numImages = 500;

load('tri66.mat');
load('pdm_aligned.mat');
load('C:\dataset\mpie-frontal.mat', 'landmarkLocations');
m2d = MtoMean2D(M);

[ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, m2d);

mpie = matfile('C:\dataset\mpie-frontal.mat');
allWarped = zeros(numImages,151,148);

images = mpie.allExamplesColour(1:ceil(numImages/2),:,:);
imSize = size(images);
for i=1:imSize(1)
    image = images(i,:,:);
    imageLandmarks = squeeze(landmarkLocations(i,:,:));
    warped = Crop(squeeze(image)/255, imageLandmarks, T, triX, mask, alphas, betas,npix,xmin, ymin);
    allWarped(i,:,:) = warped;
end

images = mpie.allExamplesColour(ceil(numImages/2)+1:numImages,:,:);
imSize = size(images);
for i=1:imSize(1)
    image = images(i,:,:);
    imageLandmarks = squeeze(landmarkLocations(i,:,:));
    warped = Crop(squeeze(image)/255, imageLandmarks, T, triX, mask, alphas, betas,npix,xmin, ymin);
    allWarped(i,:,:) = warped;
end

for i=1:numImages
    imshow(squeeze(allWarped(i,:,:)));
    waitforbuttonpress;
end