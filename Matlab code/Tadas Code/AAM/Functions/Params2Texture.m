function [ Texture ] = Params2Texture( PrincipalComponents, Bg, MeanTexture, Translate, Scale )
%PARAMS2TEXTURE Summary of this function goes here
%   Detailed explanation goes here

    TextureOffset = PrincipalComponents * Bg;    

    Texture = zeros(numel(TextureOffset),1);
    if(size(MeanTexture,2) == 1)
        Texture = MeanTexture + TextureOffset;
    else        
        for d = 1:size(MeanTexture,2)
            st = numel(MeanTexture(:,d))*(d-1)+1;
            en = numel(MeanTexture(:,d))*d;
            Texture(st:en) = MeanTexture(:,d) + TextureOffset(st:en);
        end
    end
       
    if(numel(Translate) == 1)
        Texture = Texture * Scale;
        Texture = Texture + Translate;
    else
        for d = 1:size(MeanTexture,2)
            st = numel(MeanTexture(:,d))*(d-1)+1;
            en = numel(MeanTexture(:,d))*d;
            Texture(st:en) = Texture(st:en) * Scale(d);
            Texture(st:en) = Texture(st:en) + Translate(d);
        end        
    end
end

