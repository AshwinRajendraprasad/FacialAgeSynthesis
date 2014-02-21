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
    
    [SimpleAgeSynth.male.younger.del_b, SimpleAgeSynth.male.older.del_b] = AgeingWeightedSum(textures_m, AppModel_m, AgeingModel, subjectlist, subj_numbers(male_inds));
    [SimpleAgeSynth.female.younger.del_b, SimpleAgeSynth.female.older.del_b] = AgeingWeightedSum(textures_f, AppModel_f, AgeingModel, subjectlist, subj_numbers(female_inds));

    % Find the factor that changes age by one year
    SimpleAgeSynth.male.younger.fac = FindAgeSynthFactor(textures_m, AppModel_m, AgeingModel, SimpleAgeSynth.male.younger.del_b, false);
    SimpleAgeSynth.male.older.fac = FindAgeSynthFactor(textures_m, AppModel_m, AgeingModel, SimpleAgeSynth.male.older.del_b, true);
    SimpleAgeSynth.female.younger.fac = FindAgeSynthFactor(textures_f, AppModel_f, AgeingModel, SimpleAgeSynth.female.younger.del_b, false);
    SimpleAgeSynth.female.older.fac = FindAgeSynthFactor(textures_f, AppModel_f, AgeingModel, SimpleAgeSynth.female.older.del_b, true);
end

