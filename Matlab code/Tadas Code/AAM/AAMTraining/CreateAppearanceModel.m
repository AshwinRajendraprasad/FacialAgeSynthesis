function AppearanceModel = CreateAppearanceModel( TrainingData, ShapeModel, options )
%CREATEAPPEARANCEMODEL Summary of this function goes here
%   Detailed explanation goes here

    %% Warping the model

    % convert the mean shape to texture coords        
    
    %convert base shape to 0..1 range
    baseShape(:,1) = (ShapeModel.MeanShape(:,1) - min(ShapeModel.MeanShape(:,1)));
    baseShape(:,1) = baseShape(:,1)./max(baseShape(:,1));
    baseShape(:,2) = (ShapeModel.MeanShape(:,2) - min(ShapeModel.MeanShape(:,2)));
    baseShape(:,2) = baseShape(:,2)./max(baseShape(:,2));
    
    % now translate these to texture coords
    baseShape(:,1) = 1 + baseShape(:,1) * (options.TextureSize(1)-1);
    baseShape(:,2) = 1 + baseShape(:,2) * (options.TextureSize(2)-1);
    
    uv = [ baseShape(:,1) baseShape(:,2) ];

    % create a delaunay triangulation
    dt = DelaunayTri(uv(:,1), uv(:,2));                
    
    AppearanceModel.Triangulation = dt;
    AppearanceModel.ControlPoints = uv;
    AppearanceModel.TextureDimensions = options.TextureSize;         
    
    for i=1:TrainingData.NumExamples
    
        currShape = [TrainingData.xVals(i,:); TrainingData.yVals(i,:) ]';

        currImage(:,:,:) = TrainingData.Images(i).I;
        
        if( i == 1)
           [textureVector, Map] = CreateVectorFromAppearance(currImage, currShape, baseShape, options.TextureSize,dt,options);  
           textureVectors = zeros(numel(textureVector), TrainingData.NumExamples); 
        end
                     
        [textureVectors(:,i), ~] = CreateVectorFromAppearance(currImage, currShape, baseShape, options.TextureSize,dt,options);  
         
    end
    
    AppearanceModel.BandSize = numel(find(Map));
    AppearanceModel.NumBands = size(currImage, 3);
    AppearanceModel.BandNormalisation = options.BandNormalisation;
    
    % Normalise and extract the mean texture of the appearance model (if normalisation
    % global will have only one mean if per channel will have as many means
    % as there are channels)
    [AppearanceModel.MeanTexture, AppearanceModel.TextureVectors, AppearanceModel.Transform] = NormaliseTextures(textureVectors, AppearanceModel.NumBands, AppearanceModel.BandNormalisation);
        
    AppearanceModel.TextureMap = Map;
    
    % need to organise the appearances in colums with each columd
    % representing an example, in each colour
    
    [AppearanceModel.PrincipalComponents, AppearanceModel.Variances] = PrincipalComponentAnalysis(AppearanceModel.TextureVectors, options.PreserveAppearanceVariation);
        
end

