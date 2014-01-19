function [ error ] = TestAgeingModel( AgeingModel, AppearanceModel, test_images, test_subj_numbers, subjectlist )
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
    
    YEAR_TAKEN = 2008;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);
    
    error = zeros(size(test_images,1),2);
    for i=1:size(test_images, 1)
        params = FindModelParameters(AppearanceModel, test_images(i,:));
        age_est = round(PredictAge(AgeingModel, params));
        birthyear = subjlist_year(subjlist_num==test_subj_numbers(i));
        age_real = YEAR_TAKEN - birthyear;
        error(i, 1) = age_real;
        error(i, 2) = age_est - age_real;
    end

end

