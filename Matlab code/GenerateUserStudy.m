function [ info ] = GenerateUserStudy( Model, images, tex_agerange, chosen, landmarks, M, T, mask, pathname )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

    quantity = size(tex_agerange, 2);
    
    info(quantity).age = 0;
    info(quantity).target_ages = 0;
    info(quantity).age_est = 0;
    info(quantity).perm = 0;
    
    for i=1:quantity
        ims = images(tex_agerange(i).indices);
        lms = landmarks(tex_agerange(i).indices, :, :);
        info(i).age = tex_agerange(i).ages(chosen(i));
        % generate 2 random ages, one above and one below, do this by
        % generating 2 random offsets.  Min age 16, max 94 (2 more than
        % oldest, 2 less than youngest), min change of 2 years, max of 32
        less = rand(1,1) * (min(30, info(i).age - 16 - 2)) + 2;
        more = rand(1,1) * (min(30, 94 - info(i).age - 2)) + 2;
        
        target_ages(1) = info(i).age - less;
        target_ages(2) = info(i).age + more;
        
        info(i).target_ages = target_ages;
        
        [fig, info(i).perm, info(i).age_est] = AgeSynthFigure(Model,...
            ims(chosen(i)).image, tex_agerange(i).textures(chosen(i),:),...
            tex_agerange(i).genders{chosen(i)}, info(i).target_ages, mask, M, T, squeeze(lms(chosen(i),:,:)));
        
        % save the figure as pdf in landscape mode
        set(fig,'PaperOrientation','landscape');
        set(fig,'PaperPosition', [1 1 28 19]);
        print(fig, '-dpdf', strcat(pathname, '\UserStudy', num2str(i), '.pdf'));
        close(fig);
    end

end

