errors_male = zeros(10,5504,2);
errors_female = zeros(10,5004,2);
errors_nongender = zeros(10,5504,2);

numberim_male = zeros(10,1);
numberim_female = zeros(10,1);

AppearanceModel_nongender = BuildAppearanceModel(allWarped1, true);

for i=1:10
    [~,~, diff_male, diff_female] = TrainAndTestAgeingModel_Gender(allWarped1, AppearanceModel, subjectlist, subj_numbers, genders);
    errors_male(i,1:size(diff_male,1),:) = diff_male;
    numberim_male(i) = size(diff_male,1);
    errors_female(i,1:size(diff_female,1),:) = diff_female;
    numberim_female(i) = size(diff_female,1);
%     errors_male(i) = mean(abs(diff_male(:,2)));
%     errors_female(i) = mean(abs(diff_female(:,2)));
    
    [~, diff_nongender] = TrainAndTestAgeingModel(allWarped1, AppearanceModel_nongender, subj_numbers, subjectlist);
    errors_nongender(i,1:size(diff_nongender,1),:) = diff_nongender;
end

%%
mean_error_male = zeros(10,1);
mean_error_female = zeros(10,1);
mean_error_nongender = zeros(10,1);

for i=1:10
    errors = errors_male(i,1:numberim_male(i),2);
    mean_error_male(i) = mean(abs(errors));
    errors = errors_female(i,1:numberim_female(i),2);
    mean_error_female(i) = mean(abs(errors));
    
    mean_error_nongender(i) = mean(abs(errors_nongender(i,:,2)));
end