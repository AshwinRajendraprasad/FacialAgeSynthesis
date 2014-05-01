function [ min_lam ] = CrossValidation_ageEst( trainTextures, trainNumbers, subjects, AppearanceModel )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

    min_lambdas = zeros(10,1);
    for i=1:10
        all_numbers = unique(trainNumbers);
        numCrossValid = floor(size(all_numbers,1)/10);
        % Take the next split of textures
        validate_subjects = all_numbers((i-1)*numCrossValid+1:i*numCrossValid);
        
        % Create logical array of the textures in each set
        validate_tex_inds = false(size(trainNumbers));

        for j=1:size(validate_subjects)
            validate_tex_inds = validate_tex_inds | trainNumbers==validate_subjects(j);
        end

        % split the textures and store in correct struct
        % the training textures are simply the ones not used for validation
        trainT = trainTextures(~validate_tex_inds,:);
        validateT = trainTextures(validate_tex_inds,:);
        
        % train and test the ageing model - results in 100 different
        % lambdas
        AgeingModel = BuildAgeingModel_reg(trainT, AppearanceModel, subjects, trainNumbers(~validate_tex_inds));
        error = TestAgeingModel_reg(AgeingModel, AppearanceModel, validateT, trainNumbers(validate_tex_inds), subjects);
        
        % Find the lambda that mins the abs error over the validation
        % textures
        error = error(:,:,2);
        mean_error_reg = zeros(100,1);
        for j=1:size(error,1)
            mean_error_reg(j) = mean(abs(error(j,:)));
        end
        min_lambdas(i) = min(mean_error_reg);
    end
    
    min_lam = mean(min_lambdas);
end

