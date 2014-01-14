function [ Ws ] = CreateWeightMatrix( StatisticalModel )
%CREATEWEIGHTMATRIX Summary of this function goes here
%   Detailed explanation goes here

    % Use the simplest approach to the creation of the weighting matrix,
    % just see how much variation each model has and use that as a mapping
    % same as done in AAM-API (Mikkel Stegmann)
    
    VarShape = sum(StatisticalModel.ShapeModel.Variances);
    VarAppearance = sum(StatisticalModel.AppearanceModel.Variances);

    r = sqrt(VarAppearance/VarShape);
    
    Ws = diag(repmat(r, numel(StatisticalModel.ShapeModel.Variances),1));
    
end

