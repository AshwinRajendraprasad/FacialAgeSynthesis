function [ TextureVector, Map ] = CreateVectorFromAppearance( OriginalImage, OrigShape, NewShape, Size, DT, options )
%CREATEVECTORFROMAPPEARANCE Summary of this function goes here
%   Detailed explanation goes here

    if(nargin > 5)
        [Texture, Map] = PieceWiseQuick(OriginalImage, OrigShape, NewShape, Size, DT, options.Interpolation);
    else
        [Texture, Map] = PieceWiseQuick(OriginalImage, OrigShape, NewShape, Size, DT, 'BI');
    end
    
    if( nargin > 5)
        if(options.verbose)
           imshow(options.ConvertFrom(Texture));
           drawnow;
        end
    end
    
    % Flatten the texture if more than one band
    d = size(OriginalImage, 3);
    
    sBandSize = numel(find(Map));
    
    TextureVector = zeros(sBandSize * d, 1);
    
    for i = 1:d
        Tex = Texture(:,:,i);
        TextureVector(sBandSize * (i-1) + 1: sBandSize * i) = Tex(Map);
    end
    
end

