function [ aged_face ] = AgeSynthesis_2stage( AgeSynthModel, novel_texture, target_age, current_age, gender, AgeingModel, AppearanceModel, mask, numChannels )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    if gender=='m'
        proto = AgeSynthModel.prototypes.male;
        simple = AgeSynthModel.simple.male;
    else
        proto = AgeSynthModel.prototypes.female;
        simple = AgeSynthModel.simple.female;
    end
    
    % apply the simple ageing part
    b = FindModelParameters(AppearanceModel, novel_texture);
    del_age = target_age - current_age;
    if del_age<0
        aged_b = b + (-del_age/50)*simple.younger;
    else
        aged_b = b + (del_age/50)*simple.older;
    end
    
    % transfer the high-frequency components to age more
    aged_face = SynthesiseFaceAge(proto, AppearanceParams2Texture(aged_b, AppearanceModel), ...
        target_age, AgeingModel, AppearanceModel, mask, numChannels);

end

