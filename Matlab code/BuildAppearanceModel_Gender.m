function [ AppearanceModel_male, AppearanceModel_female ] = BuildAppearanceModel_Gender( training_textures, subjectlist, genders, subj_numbers )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    [male_textures, female_textures,~,~] = SplitTextures_Gender(training_textures, subjectlist, genders, subj_numbers);
    
    AppearanceModel_male = BuildAppearanceModel(male_textures, true);
    AppearanceModel_female = BuildAppearanceModel(female_textures, true);

end

