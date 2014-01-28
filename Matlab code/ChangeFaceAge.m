function [ params_new ] = ChangeFaceAge( AgeingModel, AppearanceModel, image, inverse_ageing, age_change )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

    params = FindModelParameters(AppearanceModel, image);
    age_est = round(PredictAge(AgeingModel, params));
    display(age_est);
    display(age_change);
    
    if (age_est+age_change>size(inverse_ageing,1)) || (age_est+age_change<find(inverse_ageing,1,'first'))
        error('Age not in range');
    end
    
    params_new = params + (inverse_ageing(age_est+age_change) - inverse_ageing(age_est));
end

