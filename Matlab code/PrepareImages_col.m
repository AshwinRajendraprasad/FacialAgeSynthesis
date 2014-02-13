function [ textures, mask ] = PrepareImages_col( images, landmarkLocations, M, T )
    m2d = MtoMean2D(M);

    [ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, m2d);
    
    textures = WarpImages_col(images, landmarkLocations, T, triX, mask, alphas, betas, npix, xmin, ymin);

end