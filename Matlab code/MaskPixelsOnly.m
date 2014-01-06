function [ masked_image ] = MaskPixelsOnly( image, mask )
%MaskPixelsOnly Only stores the pixels that are in the mask, saving memory
%that would be used on zeros
    mask = mask(:);
    image_counter = 1;
    % Want the masked_image to be the size of the number of 1's in the
    % mask
    masked_image = zeros(1,sum(mask));
    for i=1:size(mask)
        if (mask(i) == 1)
            masked_image(image_counter) = image(i);
            image_counter = image_counter+1;
        end
    end

end

