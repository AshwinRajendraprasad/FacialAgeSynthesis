function [ textures_agerange ] = SplitTextures_AgeRange_general( textures, genders, ages, num_wanted )
%SplitTextures_AgeRange_general Splits the input textures into separate age ranges

    bucket_size = (max(ages) - min(ages))/num_wanted;
    
    textures_agerange = struct;
    for i=1:num_wanted
        % put the textures that are in this age range into the correct
        % element of the struct array
        indices = ages>((i-1)*bucket_size+min(ages)-1) & ages<(i*bucket_size+min(ages));
        textures_agerange(i).textures = textures(indices, :);
        textures_agerange(i).genders = genders(indices);
        textures_agerange(i).ages = ages(indices);
        textures_agerange(i).indices = indices;
    end
    
end

