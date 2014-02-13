function [ age_prototype ] = AgePrototype_singlerange( textures, sigma, mask, numChannels )
%AgePrototype_singlerange Build the age prototype for a single age range
%   Gives blurring at the edges as trying to remove the very high
%   frequency components in that region or they overwhelm the image

    avg_face = mean(textures);
    high_frequency = ones(1,size(textures,2));

%     textures = (textures)./repmat(max(abs(textures),[],2), 1, size(textures,2));
%     textures = (textures)./repmat(max(abs(textures)), size(textures,1), 1);
    
    for i=1:size(textures,1)
        high_frequency = high_frequency .* (textures(i,:) ./ GaussianBlur(textures(i,:),sigma,mask, numChannels)');
    end
    % This is removing the high frequnecy components at the edges, however it doesn't do quite
    % enough, so also have the blanket maximum for high frequency
    mask_edge = edge(mask, 'sobel');
    se = strel('diamond', 2);
    mask_edge = imdilate(mask_edge, se);
    
    high_f_im = AddZerosToImage(mask, high_frequency, numChannels);
    if numChannels==1
        high_f_im(mask_edge) = 1;
        high_f_im = high_f_im(logical(mask));
    else
        % need to do per channel
        high_f_texture = zeros(sum(mask(:)), 3);
        for i=1:numChannels
            im = high_f_im(:,:,i);
            im(mask_edge) = 1;
            high_f_texture(:,i) = im(logical(mask));
        end
        high_f_im = high_f_texture;
    end
    high_frequency = high_f_im(:);
    high_frequency(high_frequency>2) = 2;
    high_frequency(high_frequency<-2) = -2;
    
    age_prototype = high_frequency' .* avg_face;
end

