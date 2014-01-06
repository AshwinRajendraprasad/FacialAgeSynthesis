function [ V, D, mean ] = ManualPCA( allWarped, proportion, mask )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    mask_down = imresize(mask, 0.25);
    downsampled = zeros(size(allWarped,1), size(mask_down,1)*size(mask_down,2));
    for i=1:size(allWarped,1)
        image_down = imresize(reshape(allWarped(i,:), size(mask)), 0.25);
        downsampled(i,:) = image_down(:);
    end

    [mean, aligned_images, aligned_mean] = FindMean(downsampled);
    
%     covariance = cov(aligned_images);
    covariance = zeros(size(aligned_images,2));
    for i=1:size(aligned_images,1)
        covariance = covariance + (aligned_images(i,:)-aligned_mean)*transpose(aligned_images(i,:)-aligned_mean);
    end
    covariance = covariance/(size(aligned_images,1)-1);
    
%     [V, D, explained] = pcacov(covariance);
    [V,D] = eig(covariance);
    D = diag(D);
end

