function DisplayVariationsAppearance( Model, options)

ShapeModel = Model.ShapeModel;
AppearanceModel = Model.AppearanceModel;

if(options.NumParamsToShow > size(AppearanceModel.Variances))
   numParams = size(AppearanceModel.Variances);
else
   numParams = options.NumComponentsToShow; 
end

numExamples = options.NumParamsToShow;

figure;

% from -3 to 3 sqrt(var)
inc = 6 / (numExamples - 1);

% scale the texture vectors to 0 - 1 range so that they could be
% visualised

% % Dealing with global channel variation
% if(size(AppearanceModel.MeanTexture,2) == 1)
%     translationFactor = AppearanceModel.Transform.TranslateGlobal;
%     scalingFactor =  AppearanceModel.Transform.ScaleGlobal;
% else
%     for d = 1:AppearanceModel.NumBands
%         translationFactor(d) = AppearanceModel.Transform.TranslateGlobal(d);
%         scalingFactor(d) =  AppearanceModel.Transform.ScaleGlobal(d);
%     end
% end

shapeTranslationFactor = max(ShapeModel.MeanShape(:)) - min(ShapeModel.MeanShape(:));

for i=1:numParams
   
    for j=1:numExamples
        
       currPlot = subplot(numParams, numExamples, (i-1)*numExamples + j);
       % get the shape
       
       params = zeros(size(AppearanceModel.Variances));
       params(i) = sqrt(AppearanceModel.Variances(i))*(-3 + inc*(j-1));
       
       ImageToDrawOn = zeros([ AppearanceModel.TextureDimensions 3]);
       
       % sx, sy, tx, ty
       T = [ (AppearanceModel.TextureDimensions(1) - 0.15 * AppearanceModel.TextureDimensions(1)) / shapeTranslationFactor, 0, AppearanceModel.TextureDimensions(1) / 2, AppearanceModel.TextureDimensions(2) / 2 ];
       
       ImageToDrawOn = DrawTextureOnTop(ImageToDrawOn, Model, zeros(numel(ShapeModel.Variances), 1), params, T, AppearanceModel.Transform.TranslateGlobal, AppearanceModel.Transform.ScaleGlobal); 
       %ImageToDrawOn = DrawTriangulation(ImageToDrawOn, AppearanceModel.Triangulation);
       imshow(ImageToDrawOn);
       
       
%        offset = AppearanceModel.PrincipalComponents*params;
%        
%         if(size(AppearanceModel.MeanTexture,2) == 1)
%             texture = AppearanceModel.MeanTexture + offset;
%             texture = (texture * scalingFactor) + translationFactor;
%         else
%             
%             for d = 1:AppearanceModel.NumBands
%                 st = AppearanceModel.BandSize*(d-1)+1;
%                 en = AppearanceModel.BandSize*d;
% 
%                 texture(st:en) = AppearanceModel.MeanTexture(:,d) + offset(st:en);
%                 
%                 texture(st:en) = (texture(st:en) * scalingFactor(d)) + translationFactor(d) ;
% 
%             end
%         end
%         
%        
%        % now that the textures have been extracted can perform PCA       
%        tex = Vec2TexCol(texture,AppearanceModel.TextureMap,AppearanceModel.TextureDimensions, AppearanceModel.NumBands, AppearanceModel.BandSize);
%        tex = ConvertFrom(tex);
%        imshow(tex);

       axis(currPlot, 'equal');
        
    end
    
end    

end