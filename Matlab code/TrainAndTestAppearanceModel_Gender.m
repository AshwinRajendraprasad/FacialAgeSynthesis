function [ AppModel_male, AppModel_female, diff_male, diff_female ] = TrainAndTestAppearanceModel_Gender( textures, subj_numbers, genders, subjectlist )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

    [male_textures, female_textures, male_inds, female_inds] = SplitTextures_Gender(textures, subjectlist, genders, subj_numbers);
    
    [AppModel_male, diff_male] = TrainAndTestAppearanceModel(male_textures, subj_numbers(male_inds), true);
    [AppModel_female, diff_female] = TrainAndTestAppearanceModel(female_textures, subj_numbers(female_inds), true);

end

