function [ AgeingModel_male, AgeingModel_female, diff_male, diff_female ] = TrainAndTestAgeingModel_Gender( textures, AppearanceModels, subjectlist, subj_numbers, genders )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
    [male_textures, female_textures, male_inds, female_inds] = SplitTextures_Gender(textures, subjectlist, genders, subj_numbers);
    
    [AgeingModel_male, diff_male] = TrainAndTestAgeingModel(male_textures, AppearanceModels.Male, subj_numbers(male_inds), subjectlist, 0);
    [AgeingModel_female, diff_female] = TrainAndTestAgeingModel(female_textures, AppearanceModels.Female, subj_numbers(female_inds), subjectlist, 0);


end

