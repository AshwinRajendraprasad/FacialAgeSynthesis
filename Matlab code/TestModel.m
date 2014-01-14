function [ error ] = TestModel( mdl, scale, offset, test_images, test_numbs, subjectlist, modes_inv, gbar )
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
    
    YEAR_TAKEN = 2008;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);
    
    error = zeros(size(test_images,1),1);
    for i=1:size(test_images, 1)
        params = FindModelParameters(modes_inv, gbar, test_images(i,:));
        params = (params - offset) .* scale;
        age_est = round(predict(mdl, params));
        birthyear = subjlist_year(subjlist_num==test_numbs(i));
        age_real = YEAR_TAKEN - birthyear;
        error(i) = age_est - age_real;
    end

end

