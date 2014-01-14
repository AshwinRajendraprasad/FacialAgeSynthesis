function [ AppearanceModel ] = BuildAppearanceModel( training_data )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    [AppearanceModel.mean_texture, AppearanceModel.aligned_textures, AppearanceModel.Transform] = FindMean_v2(training_data);
    [AppearanceModel.modes, AppearanceModel.variances] = PerformPCA(AppearanceModel.aligned_textures, 0.98);
    
%     AppearanceModel.mean_texture = mean(training_data,1);
%     [AppearanceModel.modes, AppearanceModel.variances] = PerformPCA(training_data, 0.98);

end

