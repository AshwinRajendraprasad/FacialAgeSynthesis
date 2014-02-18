function [ SimpleAgeSynth ] = SimpleAgeSynth_gender( textures, AppearanceModel, AgeingModel, subjectlist, subj_numbers, genders, genderspecific )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

    if genderspecific
        AppModel_m = AppearanceModel.m;
        AppModel_f = AppearanceModel.f;
    else
        AppModel_m = AppearanceModel;
        AppModel_f = AppearanceModel;
    end
    
    [textures_m, textures_f, male_inds, female_inds] = SplitTextures_Gender(textures, subjectlist, genders, subj_numbers);
    
    [SimpleAgeSynth.male.younger, SimpleAgeSynth.male.older] = AgeingWeightedSum(textures_m, AppModel_m, AgeingModel, subjectlist, subj_numbers(male_inds));
    [SimpleAgeSynth.female.younger, SimpleAgeSynth.female.older] = AgeingWeightedSum(textures_f, AppModel_f, AgeingModel, subjectlist, subj_numbers(female_inds));

end

