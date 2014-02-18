function [ del_T_younger, del_T_older ] = AgeingWeightedSum( textures, AppearanceModel, AgeingModel, subjectlist, subj_numbers )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

    YEAR_TAKEN = 2008;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);

    del_T_younger = zeros(size(AppearanceModel.variances,1),1);
    del_T_older = zeros(size(AppearanceModel.variances,1),1);
    num_y = 0;
    num_o = 0;
%     average_age = mean(YEAR_TAKEN-subjlist_year);
    average_age = PredictAge(AgeingModel, zeros(size(AppearanceModel.variances)));
    ages = YEAR_TAKEN - subjlist_year;
    del_ages = (ages - average_age);
%     del_ages = del_ages/max(abs(del_ages));
    for i=1:size(textures,1)
        del_age = del_ages(subjlist_num==subj_numbers(i));
        if del_age<0
            del_T_younger = del_T_younger + (-del_age)*FindModelParameters(AppearanceModel, textures(i,:));
            num_y = num_y+1;
        else
            del_T_older = del_T_older + del_age*FindModelParameters(AppearanceModel, textures(i,:));
            num_o = num_o+1;
        end
    end
    
     del_T_younger = del_T_younger/norm(del_T_younger);
     del_T_older = del_T_older/norm(del_T_older);
end 
