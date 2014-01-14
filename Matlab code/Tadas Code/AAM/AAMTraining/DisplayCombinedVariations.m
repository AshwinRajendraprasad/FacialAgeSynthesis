function DisplayCombinedVariations(StatisticalModel, options)
%DISPLAYCOMBINEDVARIATIONS Summary of this function goes here
%   Detailed explanation goes here

if(options.NumParamsToShow > size(StatisticalModel.AppearanceModel.Variances))
   numParams = size(StatisticalModel.AppearanceModel.Variances);
else
   numParams = options.NumComponentsToShow; 
end

numExamples = options.NumParamsToShow;

figure;

% from -3 to 3 sqrt(var)
inc = 6 / (numExamples - 1);

% scale the texture vectors so that they could be
% visualised (assuming that the generated models would have the same mean
% as those that generated them)
% Dealing with global channel variation
if(size(StatisticalModel.AppearanceModel.MeanTexture,2) == 1)
    translationFactor = StatisticalModel.AppearanceModel.Transform.TranslateGlobal;
    scalingFactor =  StatisticalModel.AppearanceModel.Transform.ScaleGlobal;
else
    for d = 1:StatisticalModel.AppearanceModel.NumBands
        translationFactor(d) = StatisticalModel.AppearanceModel.Transform.TranslateGlobal(d);
        scalingFactor(d) =  StatisticalModel.AppearanceModel.Transform.ScaleGlobal(d);
    end
end

% scale the position vectors to 0 - 1 range for visualisation (same
% assumption as for the texture

% Have mean shape lie between 0 - 1

shapeTranslationFactorX = -min(StatisticalModel.ShapeModel.MeanShape(:,1));
shapeTranslationFactorY = -min(StatisticalModel.ShapeModel.MeanShape(:,2));

shapeScalingFactorX  = max(StatisticalModel.ShapeModel.MeanShape(:,1)) + shapeTranslationFactorX;
shapeScalingFactorY = max(StatisticalModel.ShapeModel.MeanShape(:,2)) + shapeTranslationFactorY;

% create a delaunay triangulation
dt = DelaunayTri(StatisticalModel.ShapeModel.MeanShape(:,1), StatisticalModel.ShapeModel.MeanShape(:,2));
tri = dt.Triangulation;    

for i=1:numParams
   
    for j=1:numExamples
        
       currPlot = subplot(numParams, numExamples, (i-1)*numExamples + j);
       
       % extract shape and appearance parameters from the combined
       % parameters
       params = zeros(size(StatisticalModel.CombinedModel.Variances));
       params(i) = sqrt(StatisticalModel.CombinedModel.Variances(i))*(-3 + inc*(j-1));
       
       Qs = StatisticalModel.ShapeModel.PrincipalComponents * (inv(StatisticalModel.CombinedModel.Ws) * StatisticalModel.CombinedModel.PrincipalComponents(1:numel(StatisticalModel.ShapeModel.Variances),:));
       Qg = StatisticalModel.AppearanceModel.PrincipalComponents * StatisticalModel.CombinedModel.PrincipalComponents(numel(StatisticalModel.ShapeModel.Variances)+1:end,:);       
       
       offsetShape = Qs * params;             
       offsetAppearance = Qg * params;
       
       % Get the texture
        if(size(StatisticalModel.AppearanceModel.MeanTexture,2) == 1)
            texture = StatisticalModel.AppearanceModel.MeanTexture + offsetAppearance;
            texture = (texture * scalingFactor) + translationFactor;
        else

            for d = 1:StatisticalModel.AppearanceModel.NumBands
                st = StatisticalModel.AppearanceModel.BandSize*(d-1)+1;
                en = StatisticalModel.AppearanceModel.BandSize*d;

                texture(st:en) = StatisticalModel.AppearanceModel.MeanTexture(:,d) + offsetAppearance(st:en);

                texture(st:en) = (texture(st:en) * scalingFactor(d)) + translationFactor(d) ;

            end
        end
        
       texture = Vec2TexCol(texture, StatisticalModel.AppearanceModel.TextureMap, StatisticalModel.AppearanceModel.TextureDimensions, StatisticalModel.AppearanceModel.NumBands, StatisticalModel.AppearanceModel.BandSize);
       texture = StatisticalModel.ConvertFrom(texture);
       % Display the variations of texture       
       
       % get the shape
       shape = [StatisticalModel.ShapeModel.MeanShape(1:end/2) StatisticalModel.ShapeModel.MeanShape(end/2+1:end)]' + offsetShape;
       
       shape(1:end/2) = options.TextureSize(1) * (shape(1:end/2) + shapeTranslationFactorX)/shapeScalingFactorX;
       shape(end/2+1:end) = options.TextureSize(2) * (shape(end/2+1:end,:) + shapeTranslationFactorY)/shapeScalingFactorY;
       
       % The shape has to be translated to 0 - 1 range  
       
       % Transform shape so that 1st col is x and second y
       shape = [shape(1:end/2)'; shape(end/2+1:end,:)']';
       
        % preprocess the image (slow but was already there, so will have to do for now)    
       % [TriangleMap, WarpParamsMap] = AffinePreProcess(tri, shape, options.TextureSize);

       meanShape = [StatisticalModel.ShapeModel.MeanShape(1:end/2) StatisticalModel.ShapeModel.MeanShape(end/2+1:end)]';
       
       meanShape(1:end/2) = options.TextureSize(1) * (meanShape(1:end/2) + shapeTranslationFactorX)/shapeScalingFactorX;
       meanShape(end/2+1:end) = options.TextureSize(2) * (meanShape(end/2+1:end,:) + shapeTranslationFactorY)/shapeScalingFactorY;

       meanShape = [meanShape(1:end/2)'; meanShape(end/2+1:end,:)']';       
       
       J = PieceWiseQuick(texture, meanShape, shape, options.TextureSize, dt, 'BI');
                  
       imshow(J(:,:,1:end));
       
       axis(currPlot, 'equal');
       drawnow('expose');
        
    end
    
end    

end

