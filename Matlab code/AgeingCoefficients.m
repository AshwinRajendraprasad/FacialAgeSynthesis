function [ model ] = AgeingCoefficients( images, modes_inv, gbar, subjectlist )
%AgeingCoefficients Finds the ageing model based on the images
%   Fits a linear model for the ages based on appearance parameters
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
        % Only use if there is an image as lack of image means don't have
        % information about subject (not in subjectlist)
        if not(images(i,:) == 0)
            app_params(i,:) = FindModelParameters(modes_inv, gbar, images(i,:));
            birthyear = subjlist_year(subjlist_num==i);
            ages(i) = YEAR_TAKEN - birthyear;
        end
    end
    
    % remove the zero ones as these might mess up the model and don't add
    % anything
    app_params(~any(app_params,2), :) = [];
    ages(~any(ages,2), :) = [];
    
    model = fitlm(app_params, ages);
end

