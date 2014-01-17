function ShowAppModel(AppearanceModel, textures, img_to_use, mask)
    close all

    params = AppearanceModel.modes' * ((textures(img_to_use,:) - AppearanceModel.Transform.translate)/AppearanceModel.Transform.scale - AppearanceModel.mean_texture)';

    reconstructed = (AppearanceModel.mean_texture' + AppearanceModel.modes * params) * AppearanceModel.Transform.scale +  AppearanceModel.Transform.translate;

    imshow(AddZerosToImage(mask, textures(img_to_use,:)/255))
    figure
    imshow(AddZerosToImage(mask, reconstructed)/255)
end
