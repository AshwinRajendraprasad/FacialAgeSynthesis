function [ age_prototypes ] = Age_Prototype( textures_agerange, AppearanceModel, AgeingModel, mask )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    age_prototypes = struct;

    for i=1:19
        sigma = 0.1 + (i-1)*0.05;
        for j=1:size(textures_agerange, 2)
            % say the target age is half way through the age range - this
            % doesn't work so well for the last age group but see how it
            % goes
            target_age = (j-1)*5+22.5;
            age_prototypes(j).prototype(i,:) = AgePrototype_singlerange(textures_agerange(j).textures, sigma, mask);
            age = PredictAge(AgeingModel, FindModelParameters(AppearanceModel, age_prototypes(j).prototype(i,:)));
            age_prototypes(j).tau(i) = age - target_age;
        end
    end
    
    for i=1:size(age_prototypes,2)
        age_prototypes(i).best_proto = age_prototypes(i).prototype((abs(age_prototypes(i).tau)==min(abs(age_prototypes(i).tau)))',:);
    end
        
end

