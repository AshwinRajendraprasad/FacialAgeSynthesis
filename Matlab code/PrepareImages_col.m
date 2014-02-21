function [ textures, mask ] = PrepareImages_col( images, landmarkLocations, M, T )
    m2d = MtoMean2D(M);

    [ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, m2d);
    
    % For new datasets it is easier to store the images in a struct as may
    % have different dimensions to image
    if isstruct(images)
        textures = WarpImages_c_s(images, landmarkLocations, T, triX, mask, alphas, betas, npix, xmin, ymin);
    else
        textures = WarpImages_col(images, landmarkLocations, T, triX, mask, alphas, betas, npix, xmin, ymin);
    end

end