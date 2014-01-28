function [ error ] = TestAgeingModel_reg( AgeingModel, AppearanceModel, test_images, test_subj_numbers, subjectlist )
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
    
    YEAR_TAKEN = 2008;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);
    
    error = zeros(size(AgeingModel.coeffs_all,2),size(test_images,1),2);
    for j=1:size(AgeingModel.coeffs_all,2)
        AgeingModel.offset = AgeingModel.FitInfo.Intercept(j);
        AgeingModel.coeffs(:,1) = AgeingModel.coeffs_all(1:end/2,j);
        AgeingModel.coeffs(:,2) = AgeingModel.coeffs_all(end/2+1:end,j);
        for i=1:size(test_images, 1)
            params = FindModelParameters(AppearanceModel, test_images(i,:));
            age_est = round(PredictAge(AgeingModel, params));
            birthyear = subjlist_year(subjlist_num==test_subj_numbers(i));
            age_real = YEAR_TAKEN - birthyear;
            error(j, i, 1) = age_real;
            error(j, i, 2) = age_est - age_real;
        end
    end

end

