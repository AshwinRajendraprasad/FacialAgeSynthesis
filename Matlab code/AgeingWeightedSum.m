function [ del_T ] = AgeingWeightedSum( textures, AppearanceModel, subjectlist, subj_numbers )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

    YEAR_TAKEN = 2008;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);

    del_T = zeros(size(AppearanceModel.variances,1),1);
    average_age = mean(YEAR_TAKEN-subjlist_year);
    ages = YEAR_TAKEN - subjlist_year;
    del_ages = abs(ages - average_age);
%     del_ages = del_ages/max(abs(del_ages));
    for i=1:size(textures,1)
        del_age = del_ages(subjlist_num==subj_numbers(i));
        del_T = del_T + del_age*FindModelParameters(AppearanceModel, textures(i,:));
    end
    
     del_T = del_T / size(textures,1);
end

