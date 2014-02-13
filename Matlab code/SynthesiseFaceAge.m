function [ aged_face ] = SynthesiseFaceAge( age_prototypes, novel_texture, target_age, AgeingModel, AppearanceModel, mask, numChannels )
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here

    age_range_targ = ceil((target_age-19)/5);
    age_range_targ(age_range_targ>8) = 8;
    
    proto = age_prototypes(age_range_targ).best_proto;
    
    sigma = 0.5;
    while true
        aged_face = proto ./ GaussianBlur(proto, sigma, mask, numChannels)' .* GaussianBlur(novel_texture, sigma, mask, numChannels)';
        age = PredictAge(AgeingModel, FindModelParameters(AppearanceModel, aged_face));
        tau = age - target_age;
        if tau>3
            if sigma>0.1
                sigma=sigma-0.1;
            else
                if age_range_targ<=1
                    break;
                end
                proto = age_prototypes(age_range_targ-1).best_proto;
                age_range_targ = age_range_targ-1;
                sigma=0.5;
            end
        elseif tau<-3
            if sigma<1.0
                sigma=sigma+0.1;
            else
                if age_range_targ>=8
                    break;
                end
                proto = age_prototypes(age_range_targ+1).best_proto;
                age_range_targ = age_range_targ+1;
                sigma=0.5;
            end
        else
            break;
        end
    end
end

