function [ Model ] = BuildAgeingModel( textures, AppearanceModel, subjectlist, subj_numbers, alpha )
%BuildAgeingModel Finds the ageing model based on the images
%   Fits a model for the ages based on appearance parameters.  The
%   subject numbers for the images are in a separate variable as there is
%   more than one image per subject
%   The output is a model from which the coefficients can be taken
    
    % Using an approximation to the subjects age by assuming the pictures
    % were taken the year that the paper was published, and calculating the
    % age as year taken - birth year
    YEAR_TAKEN = 2008;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);
    
    app_params = zeros(size(textures,1), size(AppearanceModel.modes,2));
    ages = zeros(size(textures,1),1);
    
    for i=1:size(textures,1)
        app_params(i,:) = FindModelParameters(AppearanceModel, textures(i,:));
        birthyear = subjlist_year(subjlist_num==subj_numbers(i));
        ages(i) = YEAR_TAKEN - birthyear;
        
%         load('C:\datasetageing.mat', 'mask')
%         subplot(1,2,1), subimage(AddZerosToImage(mask, AppearanceParams2Texture(app_params(i,:)', AppearanceModel))/255);
%         subplot(1,2,2), subimage(AddZerosToImage(mask, textures(i,:))/255);
    end
    
    % normalise the parameters - feature scaling
%     offset = min(app_params, [], 1);
%     app_params = app_params - repmat(offset, size(app_params,1), 1);
%     scale = 1./max(app_params, [], 1);
%     app_params = app_params .* repmat(scale, size(app_params,1), 1);
   
    Model = PolyRegression(app_params, ages, alpha);
%     Model = fitlm(app_params, ages);
end

