function [ age_prototypes ] = Age_Prototype( textures_agerange, AppearanceModel, AgeingModel, mask )
%Age_Prototype Build the age prototype for each age range
%   The age ranges are assumed to be 20-24, 25-29, ..., 55+

    MAX_SIG = 0.7;
    MIN_SIG = 0.25;
    STEP_SIG = 0.025;

    age_prototypes = struct;

    for i=1:(MAX_SIG-MIN_SIG)/STEP_SIG+1
        sigma = MIN_SIG + (i-1)*STEP_SIG;
        for j=1:size(textures_agerange, 2)
            % say the target age is half way through the age range - this
            % doesn't work so well for the last age group but see how it
            % goes
            target_age = (j-1)*5+22.5;
            age_prototypes(j).prototype(i,:) = AgePrototype_singlerange(textures_agerange(j).textures, sigma, mask);
            age = PredictAge(AgeingModel, FindModelParameters(AppearanceModel, age_prototypes(j).prototype(i,:)));
            age_prototypes(j).tau(i) = age - target_age;
            age_prototypes(j).sigma(i) = sigma;
        end
    end
    
    for i=1:size(age_prototypes,2)
        age_prototypes(i).best_proto = age_prototypes(i).prototype((abs(age_prototypes(i).tau)==min(abs(age_prototypes(i).tau)))',:);
    end
        
end

