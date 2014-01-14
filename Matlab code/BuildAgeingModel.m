function [ model, scale, offset ] = BuildAgeingModel( images, modes_inv, gbar, subjectlist, subj_numbers )
%BuildAgeingModel Finds the ageing model based on the images
%   Fits a linear model for the ages based on appearance parameters.  The
%   subject numbers for the images are in a separate variable as there is
%   more than one image per subject
%   The output is a linear model from which the coefficients can be taken
    
    % Using an approximation to the subjects age by assuming the pictures
    % were taken the year that the paper was published, and calculating the
    % age as year taken - birth year
    YEAR_TAKEN = 2008;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);
    
    app_params = zeros(size(images,1), size(modes_inv,1));
    ages = zeros(size(images,1),1);
    
    for i=1:size(images,1)
        app_params(i,:) = FindModelParameters(modes_inv, gbar, images(i,:));
        birthyear = subjlist_year(subjlist_num==subj_numbers(i));
        ages(i) = YEAR_TAKEN - birthyear;
    end
    
    % normalise the parameters so they lie between 0 and 1
    offset = min(app_params, [], 1);
    app_params = app_params - repmat(offset, size(app_params,1), 1);
    scale = 1./max(app_params, [], 1);
    app_params = app_params .* repmat(scale, size(app_params,1), 1);
    
    model = fitlm(app_params, ages);
end

