function [ params_new ] = ChangeFaceAge( mdl, offset, scale, modes_inv, gbar, image, inverse_ageing, target_age )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

    params = FindModelParameters(modes_inv, gbar, image);
    age_est = round(predict(mdl, (params-offset).*scale));
    
    params_new = params + (inverse_ageing(target_age) - inverse_ageing(age_est));
end

