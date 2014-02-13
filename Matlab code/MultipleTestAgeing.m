num_iters = 10;

% errors_male = zeros(num_iters,5504,2);
% errors_female = zeros(num_iters,5504,2);
% errors_nongender = zeros(num_iters,5504,2);
% errors_reg = zeros(num_iters, 5502, 2);
% initialise the structure
errors_r(num_iters).diff = 0;

% errors_reg = zeros(num_iters, 100, 5504, 2);
% lambdas = zeros(num_iters, 1);

% numberim_male = zeros(num_iters,1);
% numberim_female = zeros(num_iters,1);
% numberim_nongender = zeros(num_iters,1);
numberim_reg = zeros(num_iters,1);
% lambdas = zeros(num_iters, 100);

% AppearanceModel_nongender = BuildAppearanceModel(allWarped1, true);
% AppearanceModel.Male = 0;
% AppearanceModel.Female = 0;

parfor i=1:num_iters
%     [~,~, diff_male, diff_female] = TrainAndTestAgeingModel_Gender(allWarped1, AppearanceModel, subjectlist, subj_numbers, genders);
%     errors_male(i,1:size(diff_male,1),:) = diff_male;
%     numberim_male(i) = size(diff_male,1);
%     errors_female(i,1:size(diff_female,1),:) = diff_female;
%     numberim_female(i) = size(diff_female,1);
%     errors_male(i) = mean(abs(diff_male(:,2)));
%     errors_female(i) = mean(abs(diff_female(:,2)));
%     
%     [~, diff_nongender] = TrainAndTestAgeingModel(allWarped1, 0, subj_numbers, subjectlist, 0);
%     errors_nongender(i,1:size(diff_nongender,1),:) = diff_nongender;
%     numberim_nongender(i) = size(diff_nongender,1);

    [~, diff_reg] = TrainAndTestAgeingModel_reg(textures, 0, subj_numbers(1:2752), subjectlist, mean(min_lambdas));
%     errors_reg(i,1:size(diff_reg,1),:) = diff_reg;
    errors_r(i).diff = diff_reg;
    numberim_reg(i) = size(diff_reg,1);
    
%     [AM_reg, diff_reg] = TrainAndTestAgeingModel_reg(textures, 0, subj_numbers(1:2752), subjectlist, 0);
% %     errors_reg(i,:,1:size(diff_reg,2),:) = diff_reg;
%     errors_r(i).diff = diff_reg;
%     numberim_reg(i) = size(diff_reg,2);
%     lambdas(i,:) = AM_reg.FitInfo.Lambda;
    
end

%%
% mean_error_male = zeros(num_iters,1);
% mean_error_female = zeros(num_iters,1);
% mean_error_nongender = zeros(num_iters,1);
mean_error_reg = zeros(num_iters,1);
% mean_error_reg = zeros(num_iters,100);
% min_lambdas = zeros(num_iters,1);

for i=1:num_iters
%     errors = errors_male(i,1:numberim_male(i),2);
%     mean_error_male(i) = mean(abs(errors));
%     errors = errors_female(i,1:numberim_female(i),2);
%     mean_error_female(i) = mean(abs(errors));
%     
%     errors = errors_nongender(i,1:numberim_nongender(i),2);
%     mean_error_nongender(i) = mean(abs(errors));

%     errors = errors_reg(i,1:numberim_reg(i),2);
    errors = squeeze(errors_r(i).diff(:,2));
    mean_error_reg(i) = mean(abs(errors));

% %     errors = squeeze(errors_reg(i, :, 1:numberim_reg(i),2));
%     errors = errors_r(i).diff(:,:,2);
%     for j=1:100
%         mean_error_reg(i, j) = mean(abs(errors(j,:)));
%     end
%     min_lamb = min(mean_error_reg(i,:));
% 
%     min_lambdas(i) = lambdas(i,mean_error_reg(i,:) == min_lamb);

end