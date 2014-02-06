function [age_prototypes_m, age_prototypes_f] = AgePrototypes_gender(textures, subjectlist, subj_numbers, genders, mask, AppearanceModel, AgeingModel)

    [textures_male, textures_female, inds_male, inds_female] = SplitTextures_Gender(textures, subjectlist, genders, subj_numbers);
    textures_m_ageranges = SplitTextures_AgeRange(textures_male, subjectlist, subj_numbers(inds_male));
    textures_f_ageranges = SplitTextures_AgeRange(textures_female, subjectlist, subj_numbers(inds_female));

    age_prototypes_m = Age_Prototype(textures_m_ageranges, AppearanceModel, AgeingModel, mask);
    age_prototypes_f = Age_Prototype(textures_f_ageranges, AppearanceModel, AgeingModel, mask);
end