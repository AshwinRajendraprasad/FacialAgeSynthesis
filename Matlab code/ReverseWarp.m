function [ new_img ] = ReverseWarp( img, T, landmarks, M2D, orig_img, numChannels )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    % Assume that there is only one channel (makes compatible with
    % grayscale code)
    if nargin<6
        numChannels = 1;
    end
    
    % Need to use integer valued landmarks as necessary for creating
    % full-size mask
    landmarks = round(landmarks);

    % set landmarks so that they are the right scale for the image
    landmarks_zero(:,1) = landmarks(:,1) - min(landmarks(:,1))+1;
    landmarks_zero(:,2) = landmarks(:,2) - min(landmarks(:,2))+1;
    % set landmarks so they are centred at 0,0
    landmarks_zero(:,1) = landmarks_zero(:,1) - max(landmarks_zero(:,1))/2;
    landmarks_zero(:,2) = landmarks_zero(:,2) - max(landmarks_zero(:,2))/2;
    
    % set M2D so that 0,0 is the top left
    M2D(:,1) = M2D(:,1) + abs(min(M2D(:,1)));
    M2D(:,2) = M2D(:,2) + abs(min(M2D(:,2)));

    % Perform the PAW with M2D and landmarks swapped, so it will
    % effectively 'unwarp' the image
    [ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, landmarks_zero);
    if numChannels==1
        unwarped_img = Crop(img, M2D, T, triX, mask, alphas, betas, npix, xmin, ymin);
    else
        unwarped_img = zeros(size(mask,1),size(mask,2),numChannels);
        for i=1:numChannels
            unwarped_img(:,:,i) = Crop(img(:,:,i), M2D, T, triX, mask, alphas, betas, npix, xmin, ymin);
        end
    end
    
    % erode the mask to remove a black border that appears
    se = strel('diamond', 3);
    mask = imerode(mask, se);
    
    % create a mask that covers the whole image and is only 1 where the
    % face is
    full_size_mask = zeros(size(orig_img,1), size(orig_img,2));
    full_size_mask(min(landmarks(:,2)):max(landmarks(:,2)), min(landmarks(:,1)):max(landmarks(:,1))) = mask;
    
    % need to replicate the mask so that it selects the pixels in all
    % channels
    full_size_mask = repmat(full_size_mask, 1, 1, numChannels);
    mask = repmat(mask, 1, 1, numChannels);
    
    new_img = orig_img;
    new_img(logical(full_size_mask)) = unwarped_img(logical(mask));

end

