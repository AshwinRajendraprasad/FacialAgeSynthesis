function [ textures, mask ] = PrepareImages( images, landmarkLocations, M, T )
    m2d = MtoMean2D(M);

    [ alphas, betas, triX, mask, xmin, ymin, npix ] = InitialisePieceWiseAffine(T, m2d);
    
    textures = WarpImages(images, landmarkLocations, T, triX, mask, alphas, betas, npix, xmin, ymin, 1)/255;

end