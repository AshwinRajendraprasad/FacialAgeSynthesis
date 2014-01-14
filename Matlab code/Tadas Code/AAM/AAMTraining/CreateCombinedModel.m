function [ CombinedModel ] = CreateCombinedModel( StatisticalModel, options )
%CREATECOMBINEDMODEL Summary of this function goes here
%   Detailed explanation goes here

    % First need to find the mapping from shape to appearance to do varying
    % units (location, intensity)
    CombinedModel.Ws = CreateWeightMatrix(StatisticalModel);

    % Get the shape parameters for each example in the data
    MeanShape = [StatisticalModel.ShapeModel.MeanShape(1:end/2) StatisticalModel.ShapeModel.MeanShape(end/2+1:end)]';
    Xnorm = StatisticalModel.ShapeModel.NormalisedShapes - repmat(MeanShape,1, size(StatisticalModel.ShapeModel.NormalisedShapes,2));
    
    Bs = CombinedModel.Ws * (StatisticalModel.ShapeModel.PrincipalComponents') * Xnorm;
    
    % Get the appearance parameters
    MeanAppearance = StatisticalModel.AppearanceModel.MeanTexture;
    
    % Global
    Gnorm = zeros(size(StatisticalModel.AppearanceModel.TextureVectors));
    if(strcmpi(StatisticalModel.AppearanceModel.BandNormalisation, 'Global'))
        Gnorm = StatisticalModel.AppearanceModel.TextureVectors - repmat(MeanAppearance,1,size(StatisticalModel.ShapeModel.NormalisedShapes,2));
    else
        for d=1:StatisticalModel.AppearanceModel.NumBands
            
            st = StatisticalModel.AppearanceModel.BandSize*(d-1)+1;
            en = StatisticalModel.AppearanceModel.BandSize*d;

            Gnorm(st:en, :) = StatisticalModel.AppearanceModel.TextureVectors(st:en,:) - repmat(MeanAppearance(:,d),1,size(StatisticalModel.ShapeModel.NormalisedShapes,2));
        end
    end
    
    Bg = (StatisticalModel.AppearanceModel.PrincipalComponents') * Gnorm;
    
    % Combine both
    Bc = [Bs; Bg];
    
    [CombinedModel.PrincipalComponents CombinedModel.Variances] = PrincipalComponentAnalysis(Bc, options.PreserveCombinedVariation);
    
end

