function [ chosen, tex_agerange ] = FindTestSubjects( Model, textures, ages, genders, mask )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    num_wanted = 30;
    
    tex_agerange = SplitTextures_AgeRange_general(textures, genders, ages, num_wanted);
    
    chosen = zeros(num_wanted,1);
    num_male = 0;
    num_female = 0;
    
    for i=1:size(tex_agerange,2)
        % display all of the textures for this age range
        for j=1:size(tex_agerange(i).textures,1)
            % display 15 per figure
            if mod(j, 15)==1
                figure
                offset = 1;
            end
            b = FindModelParameters(Model.App, tex_agerange(i).textures(j,:));
            
            % display the original image and the model of original for
            % comparison
            subplot(5,6,(offset-1)*2+1), imshow(AddZerosToImage(mask, tex_agerange(i).textures(j,:), 3)/255);
            title(strcat(num2str(j), ' ', tex_agerange(i).genders{j}));
            subplot(5,6,(offset-1)*2+2), imshow(AddZerosToImage(mask, AppearanceParams2Texture(b, Model.App), 3)/255);
            offset = offset+1;
        end
        
        % Prompt the user for the number of the best fit (also telling them
        % ratio of male to female so far)
        in = str2double(input(strcat('Which has the best fit (',num2str(num_male),':',num2str(num_female),')','?\n'),'s'));
        chosen(i) = in;
        if tex_agerange(i).genders{in}=='m'
            num_male = num_male+1;
        else
            num_female = num_female+1;
        end
        close all
    end
    
%     indexes = zeros(num_wanted);
%     num_male = 0;
%     num_female = 0;
%     for i=1:size(textures,1)
%         b = FindModelParameters(Model.App, textures(i,:));
%         
%         % find which bucket this subject is in
%         bucket = ceil((ages(i) - min(ages)+1)/age_bucket_size);
%         
%         % if already have an image for this bucket or already have enough of this gender then don't go any
%         % further
%         % have 2:1 split for male:female (similar to the split in the data)
%         if genders{i} == 'm'
%             too_many = num_male > num_wanted/3*2;
%         else
%             too_many = num_female > num_wanted/3;
%         end
%         
%         if (indexes(bucket) ~= 0) || too_many
%             continue;
%         end
%         
%         % display the original image and the model of original for
%         % comparison
%         f = figure;
%         subplot(1,2,1), imshow(AddZerosToImage(mask, textures(i,:), 3)/255);
%         subplot(1,2,2), imshow(AddZerosToImage(mask, AppearanceParams2Texture(b, Model.App), 3)/255);
%         
%         kkey = input('', 's');
%         close(f);
%         
%         % If user presses enter then accept that image for the age range
%         if kkey == 13
%             indexes(bucket) = i;
%         end
%     end
%     
%     % throw error if didn't pick subject for any age group
%     if any(~indexes)
%         error('Didnt pick subject for each group');
%     end

end

