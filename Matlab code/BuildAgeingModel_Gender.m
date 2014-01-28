function [ AgeingModels ] = BuildAgeingModel_Gender( textures, AppearanceModels, subjectlist, subj_numbers, genders )
%BuildAgeingModel_Gender Builds separate ageing model for male and female,
%using separate appearance models for male and female
%   Detailed explanation goes here

    [male_textures, female_textures, male_inds, female_inds] = SplitTextures_Gender(textures, subjectlist, genders, subj_numbers);
    
    [AgeingModels.Male] = BuildAgeingModel(male_textures, AppearanceModels.Male, subjectlist, subj_numbers(male_inds), 0.003,0);
    [AgeingModels.Female] = BuildAgeingModel(female_textures, AppearanceModels.Female, subjectlist, subj_numbers(female_inds), 0.003,0);

end

