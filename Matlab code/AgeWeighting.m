function [ error_distance ] = AgeWeighting( weightings, images, subjectlist, modes_inv, gbar )
%AgeWeighting This function will be minimised to find the correct offset and
%weights for the quadratic aging function
%   Returns the sum of squared distance from the estimated age to the
%   actual age for all of the images in the dataset that we have an age
%   for.
%   The arguments:
%       images - the warped images
%       subjectlist - the list of subjects and birth years
%       subj_numbers - the subject number of each image
%       V_inv - inverse of the model
%       gbar - the mean face image
   
    PIC_YEAR = 2012;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);
    
    error_distance = 0;
    
    offset = weightings(1,1);
    w1 = weightings(:,2);
    w2 = weightings(:,3);
    
    for i=1:size(images,1)
        % Only use if there is an image as lack of image means don't have
        % information about subject (not in subjectlist)
        if not(images(i,:) == 0)
            birthyear = subjlist_year(subjlist_num==i);
            b = FindModelParameters(modes_inv, gbar, images(i,:));
            % the offset is a vector so that all of the arguments can be put
            % into a matrix for the minimisation
            age_est = offset + transpose(w1)*transpose(b) + transpose(w2)*transpose(b.*b);
            age_real = PIC_YEAR - birthyear;
            error_distance = error_distance + (age_real-age_est)^2;
        end
    end

end

