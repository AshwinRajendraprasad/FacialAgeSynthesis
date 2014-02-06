function [ new_img ] = ReverseWarp( img, T, landmarks, M2D, orig_img )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

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
    unwarped_img = Crop(img, M2D, T, triX, mask, alphas, betas, npix, xmin, ymin);
    
    % erode the mask to remove a black border that appears
    se = strel('diamond', 3);
    mask = imerode(mask, se);
    
    % create a mask that covers the whole image and is only 1 where the
    % face is
    full_size_mask = zeros(size(orig_img));
    landmarks = round(landmarks);
    full_size_mask(min(landmarks(:,2)):max(landmarks(:,2)), min(landmarks(:,1)):max(landmarks(:,1))) = mask;
    
    new_img = orig_img;
    new_img(logical(full_size_mask)) = unwarped_img(logical(mask));
    
end

