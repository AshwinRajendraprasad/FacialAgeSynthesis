function DisplaySimpleAgeSynth( AppModel, AgeSynthModel, mask )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    subplot(2,2,1), imshow(AddZerosToImage(mask, AppearanceParams2Texture(AgeSynthModel.male.younger, AppModel), 3)/255);
    title('Younger')
    subplot(2,2,2), imshow(AddZerosToImage(mask, AppearanceParams2Texture(AgeSynthModel.male.older, AppModel), 3)/255);
    title('Older')
    subplot(2,2,3), imshow(AddZerosToImage(mask, AppearanceParams2Texture(AgeSynthModel.female.younger, AppModel), 3)/255);
    subplot(2,2,4), imshow(AddZerosToImage(mask, AppearanceParams2Texture(AgeSynthModel.female.older, AppModel), 3)/255);
end

