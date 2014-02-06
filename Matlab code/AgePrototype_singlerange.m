function [ age_prototype ] = AgePrototype_singlerange( textures, sigma, mask )
%AgePrototype_singlerange Build the age prototype for a single age range
%   Gives blurring at the edges as trying to remove the very high
%   frequency components in that region or they overwhelm the image

    avg_face = mean(textures);
    high_frequency = ones(1,size(textures,2));

%     textures = (textures)./repmat(max(abs(textures),[],2), 1, size(textures,2));
%     textures = (textures)./repmat(max(abs(textures)), size(textures,1), 1);
    
    for i=1:size(textures,1)
        high_frequency = immultiply(high_frequency, (imdivide(textures(i,:),GaussianBlur(textures(i,:),sigma,mask)')));
    end
    % This is removing the high frequnecy components at the edges, however it doesn't do quite
    % enough, so also have the blanket maximum of 3 for high frequency
    mask_edge = edge(mask, 'sobel');
    se = strel('diamond', 2);
    mask_edge = imdilate(mask_edge, se);
    
    high_f_im = AddZerosToImage(mask, high_frequency);
    high_f_im(mask_edge) = 1;
    high_f_im = high_f_im(logical(mask));
    high_frequency = high_f_im(:);
    high_frequency(high_frequency>2) = 2;
    high_frequency(high_frequency<-2) = -2;
    
    age_prototype = immultiply(high_frequency', avg_face);
end

