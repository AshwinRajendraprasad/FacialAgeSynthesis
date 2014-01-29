function [ age_proto, age_quant ] = AverageParamsAgeRange( textures, AppearanceModel, subjectlist, subj_numbers, sigma, mask )
%UNTITLED2 Summary of this function goes here
%   The age ranges are 20-24, 25-29, ..., 55+
    
    % Using an approximation to the subjects age by assuming the pictures
    % were taken the year that the paper was published, and calculating the
    % age as year taken - birth year
    YEAR_TAKEN = 2008;
    
    subjlist_num = subjectlist(:,1);
    subjlist_year = subjectlist(:,2);
    
    mask_edge = edge(mask, 'sobel');
    se = strel('diamond', 2);
    mask_edge = imdilate(mask_edge, se);
    
%     avg_params = zeros(8, size(AppearanceModel.variances,1));
    avg_params = zeros(8, size(textures,2));
    age_quant = zeros(8,1);
%     high_frequency = zeros(8, size(AppearanceModel.variances,1));
    high_frequency = ones(8, size(textures,2));
    
    for i=1:size(textures,1)
        birthyear = subjlist_year(subjlist_num==subj_numbers(i));
        age = YEAR_TAKEN - birthyear;
        age_range = ceil((age-19)/5);
        
        if age_range>8
            age_range=8;
        end
         
%         b = FindModelParameters(AppearanceModel, textures(i,:));
%         b_low = FindModelParameters(AppearanceModel, GaussianBlur(textures(i,:),sigma,mask)');
        
%         avg_params(age_range,:) = avg_params(age_range,:) + b';
        avg_params(age_range,:) = avg_params(age_range,:) + textures(i,:);
%         high_frequency(age_range, :) = high_frequency(age_range,:) + (b-b_low)';
        high_frequency(age_range, :) = immultiply(high_frequency(age_range, :), (imdivide(textures(i,:),GaussianBlur(textures(i,:),sigma,mask)')));
        age_quant(age_range) = age_quant(age_range)+1;
    end
    % This is the removing the high frequnecy components at the edges, however it doesn't do quite
    % enough, so also have the blanket maximum of 3 for high frequency
    for i=1:8   
        high_f_im = AddZerosToImage(mask, high_frequency(i,:));
        high_f_im(mask_edge) = 1;
        high_f_im = high_f_im(logical(mask));
        high_frequency(i,:) = high_f_im(:);
    end
    high_frequency(high_frequency>3) = 3;
%     avg_params = avg_params./repmat(age_quant, 1, size(AppearanceModel.variances,1));
    avg_params = avg_params./repmat(age_quant, 1, size(textures,2));
%     high_frequency = high_frequency./repmat(age_quant, 1, size(AppearanceModel.variances,1));
    
    age_proto = immultiply(high_frequency, avg_params);
%     age_proto = avg_params + high_frequency;

end

