function [ AppearanceModel ] = BuildAppearanceModel( training_textures, single_norm_iteration )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    [AppearanceModel.mean_texture, AppearanceModel.aligned_textures, AppearanceModel.Transform] = FindMean_v2(training_textures, single_norm_iteration);
    [AppearanceModel.modes, AppearanceModel.variances] = PerformPCA(AppearanceModel.aligned_textures, 0.98);
    
%     AppearanceModel = rmfield(AppearanceModel, 'aligned_textures');

end

