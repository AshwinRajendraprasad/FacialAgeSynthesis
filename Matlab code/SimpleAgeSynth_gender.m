function [ SimpleAgeSynth ] = SimpleAgeSynth_gender( textures, AppearanceModel, subjectlist, subj_numbers, genders )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    
    [textures_m, textures_f, male_inds, female_inds] = SplitTextures_Gender(textures, subjectlist, genders, subj_numbers);
    
    SimpleAgeSynth.male = AgeingWeightedSum(textures_m, AppearanceModel, subjectlist, subj_numbers(male_inds));
    SimpleAgeSynth.female = AgeingWeightedSum(textures_f, AppearanceModel, subjectlist, subj_numbers(female_inds));

end

