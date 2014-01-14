function [ Image ] = DrawTextureOnTop( ImageToDrawOn, Model, Bs, Bg, T, TextureTranslate, TextureScale)
%DRAWTEXTUREONTOP Drawing the inferred appearance of the object based on
%the parameters provided, Model contains the means, variances and principal
%components of the model, Bs is the shape parameters, Bg texture parameters
%and T is similarity transform (sx,sy,tx,ty). sx =(s cos(a)-1) sy = s
%sin(a), where s is scale and a is rotation. U is the texture scaling and
%shifting parameters.
%   Detailed explanation goes here

    % Creating shape from parameters
    Shape = Params2Shape(Model.ShapeModel.PrincipalComponents, Bs, Model.ShapeModel.MeanShape, T);    
    
    % Creating texture from parameters
    Texture = Params2Texture(Model.AppearanceModel.PrincipalComponents, Bg, Model.AppearanceModel.MeanTexture, TextureTranslate, TextureScale);

    % Convert from an array to a 2D representation
    Texture = Vec2TexCol(Texture, Model.AppearanceModel.TextureMap, Model.AppearanceModel.TextureDimensions, Model.AppearanceModel.NumBands, Model.AppearanceModel.BandSize);

    Texture = Model.ConvertFrom(Texture);
    
    %% Map the shape and texture onto an image
    [M, N,~] = size(ImageToDrawOn);                
    
    [J, Map] = PieceWiseQuick(Texture, Model.AppearanceModel.ControlPoints, Shape, [M,N], Model.AppearanceModel.Triangulation, 'BI');                

    % Need to split into RGB for the overlay
    if(size(J,3) == 3)
        ToShowR = ImageToDrawOn(:,:,1);
        ToShowG = ImageToDrawOn(:,:,2);
        ToShowB = ImageToDrawOn(:,:,3);

        JR = J(:,:,1);
        JG = J(:,:,2);
        JB = J(:,:,3);

        ToShowR(Map) = JR(Map);
        ToShowG(Map) = JG(Map);
        ToShowB(Map) = JB(Map);

        ToShow = cat(3, ToShowR, ToShowG, ToShowB);
        Image = ToShow;
    else      
        Image = ImageToDrawOn;
        Image(Map) = J(Map);
    end
    %Image = DrawObject(Shape(:,1)',Shape(:,2)', Model.ShapeModel.paths, Model.ShapeModel.types, Image);
    
end

