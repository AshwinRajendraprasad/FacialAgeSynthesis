function DisplayVariationsShape( Model, options)
%DISPLAYVARIATIONS Displays up to 6 main variants of the shape model
%   Detailed explanation goes here

ShapeModel = Model.ShapeModel;
AppearanceModel = Model.AppearanceModel;

if(options.NumParamsToShow > size(ShapeModel.Variances))
   numParams = size(ShapeModel.Variances);
else
   numParams = options.NumComponentsToShow; 
end

numExamples = options.NumParamsToShow;

figure;

shapeTranslationFactor = max(ShapeModel.MeanShape(:)) - min(ShapeModel.MeanShape(:));

% from -3 to 3 sqrt(var)
inc = 6 / (numExamples - 1);

for i=1:numParams
   
    for j=1:numExamples
       subplot(numParams, numExamples, (i-1)*numExamples + j);
       % get the shape
       
       params = zeros(size(ShapeModel.Variances));
       params(i) = sqrt(ShapeModel.Variances(i))*(-3 + inc*(j-1));
       
       ImageToDrawOn = zeros([ AppearanceModel.TextureDimensions 3]);
       
       % sx, sy, tx, ty
       
       % Have the scaling a bit smaller to fit in the dimensions
       
       T = [ (AppearanceModel.TextureDimensions(1) - 0.15 * AppearanceModel.TextureDimensions(1)) / shapeTranslationFactor, 0, AppearanceModel.TextureDimensions(1) / 2, AppearanceModel.TextureDimensions(2) / 2 ];
       
       ImageToDrawOn = DrawTextureOnTop(ImageToDrawOn, Model, params, zeros(numel(AppearanceModel.Variances), 1), T, AppearanceModel.Transform.TranslateGlobal, AppearanceModel.Transform.ScaleGlobal); 
       
       %ImageToDrawOn = DrawTriangulation(ImageToDrawOn, AppearanceModel.Triangulation);
       
       imshow(ImageToDrawOn);           
        
    end
    
end

end

