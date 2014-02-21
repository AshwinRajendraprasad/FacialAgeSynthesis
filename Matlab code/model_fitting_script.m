numImages = size(allWarped1,1);
diffs = zeros(numImages,1);
diffs_all = zeros(numImages,1);

for i=1:numImages
    close all
    img_to_use = i;
    % img = zscore(allWarped1(60,:)'/255);

    % params_app1 = AppearanceModel1.modes' * (img - AppearanceModel1.mean_texture);
    params_all = AppearanceModel_all.modes' * ((allWarped1(img_to_use,:) - AppearanceModel_all.Transform.translate)/AppearanceModel_all.Transform.scale - AppearanceModel_all.mean_texture)';
    params = AppearanceModel.modes' * ((allWarped1(img_to_use,:) - AppearanceModel.Transform.translate)/AppearanceModel.Transform.scale - AppearanceModel.mean_texture)';

    reconstructed_all = (AppearanceModel_all.mean_texture' + AppearanceModel_all.modes * params_all) * AppearanceModel_all.Transform.scale +  AppearanceModel_all.Transform.translate;
    reconstructed = (AppearanceModel.mean_texture' + AppearanceModel.modes * params) * AppearanceModel.Transform.scale +  AppearanceModel.Transform.translate;

    diffs(i) = sum(imabsdiff(allWarped1(img_to_use,:)'/255, reconstructed/255));
    diffs_all(i) = sum(imabsdiff(allWarped1(img_to_use,:)'/255, reconstructed_all/255));

    imshow(AddZerosToImage(mask, allWarped1(img_to_use,:)/255))
    figure
    imshow(AddZerosToImage(mask, reconstructed_all)/255)
    figure
    imshow(AddZerosToImage(mask, reconstructed)/255)
end
