function [ texture ] = WarpSingleImage( image, landmarks, M, T, numChannels )
%WarpSingleImage Warp a single image from landmarks to M
%   Detailed explanation goes here
    
    % Prepare the warp
    m2d = MtoMean2D(M);
    [ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, m2d);

    % Warp each channel separately and then combine
    texture = zeros(sum(mask(:)), numChannels);
    for i=1:numChannels
        tex = Crop(image(:,:,i), landmarks, T, triX, mask, alphas, betas,npix,xmin, ymin);
        texture(:,i) = tex(logical(mask));
    end
    texture = texture(:);
end

