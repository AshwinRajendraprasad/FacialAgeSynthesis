function [ f ] = FindAgeSynthFactor( textures, AppearanceModel, AgeingModel, age_synth, older )
%FindAgeSynthFactor Finds the factor to multiply age synthesis by to change
%age by 1 year
%   Detailed explanation goes here

    epsilon = 1;
    step = 5;
    max_factor = 200;

    if older
        delta_age = 10;
    else
        delta_age = -10;
    end
    
    factors = zeros(size(textures,1),1);
    for i=1:size(textures,1)
        b = FindModelParameters(AppearanceModel, textures(i,:));
        age = round(PredictAge(AgeingModel, b));
        target_age = age + delta_age;
        
        factors(i) = 10;
        
        while abs(target_age - age) > epsilon
            if factors(i) > max_factor
                break
            else
                age = round(PredictAge(AgeingModel, b+(factors(i)*age_synth)));
                factors(i) = factors(i) + step;
            end
        end
    end
    
    % find the average factor for 10 years
    f = mean(factors);
    % the average factor for 1 year change
    f = f/10;
end

