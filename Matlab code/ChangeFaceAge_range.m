function [ params_new ] = ChangeFaceAge_range( AppearanceModel, image, avg_params, current_age, target_age )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

    params = FindModelParameters(AppearanceModel, image);
    
    age_range_cur = ceil((current_age-19)/5);
    age_range_targ = ceil((target_age-19)/5);
    
    if current_age-target_age > 0
        factor = 1/((current_age-target_age)/2);
    else
        factor = (current_age-target_age);
    end
    
    params_new = params + (avg_params(age_range_targ) - avg_params(age_range_cur));
    
end

