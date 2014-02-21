i = round(rand(1,1) * 36) + 1;

alpha = dot(textures_new(i,:), AppModel_cootes.mean_texture)/numel(textures_new(i,:));
beta = mean(textures_new(i,:));
tex = textures_new(i,:) * alpha + beta;
b = AppModel_cootes.modes' * (tex - AppModel_cootes.mean_texture)';
b_nonorm = AppModel_cootes.modes' * (textures_new(i,:)-AppModel_cootes.mean_texture)';

subplot(1,5,1),imshow(AddZerosToImage(mask, textures_new(i,:),3)/255);
subplot(1,5,2), imshow(AddZerosToImage(mask, AppearanceParams2Texture(b, AppModel_cootes), 3)/255);
subplot(1,5,3), imshow(AddZerosToImage(mask, alpha*(AppModel_cootes.mean_texture + (AppModel_cootes.modes*b)')+beta,3)/255);
subplot(1,5,4), imshow(AddZerosToImage(mask, alpha*(AppModel_cootes.mean_texture + (AppModel_cootes.modes*b_nonorm)')+beta,3)/255);
subplot(1,5,5), imshow(AddZerosToImage(mask, AppearanceParams2Texture(FindModelParameters(AppModel_cootes, textures_new(i,:)), AppModel_cootes), 3)/255);