function [ConvertTo, ConvertFrom, numberBands] = GetColourTransform( transform )
%GETCOLOURTRANSFORM Provided the name of the transform returns the simple
%matrix transforms to convert from the current bands to one specified and
%back, currently supports 'rgb', 'grayscale', 'I1I2I3'

    if(strcmpi(transform, 'rgb'))
        ConvertTo = @Identity;
        ConvertFrom = @Identity;
        numberBands = 3;
    elseif(strcmpi(transform, 'grayscale'))
        ConvertTo = @RGBToGrayscale;
        ConvertFrom = @GrayscaleToRGB;
        numberBands = 1;
    elseif(strcmpi(transform, 'i1i2i3'))
        ConvertTo = @RGBToI1I2I3;
        ConvertFrom = @I1I2I3ToRGB;        
        numberBands = 3;
    else
        fprintf('Unknown colour transform, defaulting to RGB');
        ConvertTo = @Identity;
        ConvertFrom = @Identity;
        numberBands = 3;
    end

end

function [RGBO] = Identity(RGB)
    RGBO = RGB;
end

function [Grayscale] = RGBToGrayscale(RGB)
    
    Grayscale = rgb2gray(RGB);

end

function [I1I2I3] = RGBToI1I2I3(RGB)
    
    % I1 = (R + G + B)/3
    % I2 = (R - B)/2
    % I3 = (2G - R - B) / 4
    I1I2I3 = zeros(size(RGB));
    I1I2I3(:,:,1) = (RGB(:,:,1) + RGB(:,:,2) + RGB(:,:,3)) ./ 3;
    I1I2I3(:,:,2) = (RGB(:,:,1) - RGB(:,:,3)) ./ 2;
    I1I2I3(:,:,3) = (2*RGB(:,:,2) - RGB(:,:,1) - RGB(:,:,3)) ./ 4;
    
end

function [RGB] = GrayscaleToRGB(Grayscale)
    
    RGB = zeros(size(Grayscale));
    RGB(:,:,1) = Grayscale;
    RGB(:,:,2) = Grayscale;
    RGB(:,:,3) = Grayscale;
    
end

function [RGB] = I1I2I3ToRGB(I1I2I3)
    
    % R = (3*I1 + 3*I2 - 2*I3) / 3
    % G = (3*I1 + 4*I3) / 3
    % B = (3*I1 - 3*I2 - 2*I3) / 3
    RGB = zeros(size(I1I2I3));
    RGB(:,:,1) = (3*I1I2I3(:,:,1) + 3*I1I2I3(:,:,2) - 2 * I1I2I3(:,:,3)) ./ 3;
    RGB(:,:,2) = (3*I1I2I3(:,:,1) + 4* I1I2I3(:,:,3)) ./ 3;
    RGB(:,:,3) = (3*I1I2I3(:,:,1) - 3*I1I2I3(:,:,2) - 2 * I1I2I3(:,:,3)) ./ 3;
    
end

