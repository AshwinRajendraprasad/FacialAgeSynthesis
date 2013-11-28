function [warpedImage] = Crop(inputImage, destinationPoints, triangulation, triX, mask, alphas, betas, nPix, minX, minY)

    coeffs = CalculateCoefficients(alphas, betas, triangulation, destinationPoints);
    [ mapX, mapY ] = WarpRegion( minX, minY, mask, triX, coeffs );
    warpedImage = Remap(inputImage, mapX, mapY);
end