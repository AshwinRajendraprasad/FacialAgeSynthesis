function [ textures_agerange ] = SplitTextures_AgeRange( textures, subjectlist, subj_numbers )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    YEAR_TAKEN = 2008;
    
    textures_agerange = struct;
    ages = YEAR_TAKEN-subjectlist(:,2);
    for i=1:8
        if i==8
            subj_agerange = subjectlist(ages>((i-1)*5+19));
        else
            subj_agerange = subjectlist(ages>((i-1)*5+19) & ages<(i*5+20), 1);
        end
        textures_agerange(i).textures = textures(ismember(subj_numbers,subj_agerange),:);
    end
    
end

