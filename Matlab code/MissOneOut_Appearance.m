function [  ] = MissOneOut_Appearance( textures, mask, subjectlist, subj_numbers )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    % pick a random subject
    test_subj = round(rand(1,1)*(max(subjectlist(:,1))-1)+1);
    
    % some subject numbers missing, if this is the case pick a new one
    while ~any(test_subj == subjectlist(:,1))
        test_subj = round(rand(1,1)*(max(subjectlist(:,1))-1)+1);
    end
    
    index = find(test_subj == subjectlist(:,1), 1, 'first');
    subjects = [subjectlist(1:index-1,1); subjectlist(index+1:end, 1)];
    
    inds = false(size(subj_numbers));
    for i=1:size(subjects,1)
        inds = inds | subjects(i) == subj_numbers;
    end
    
    AppearanceModel = BuildAppearanceModel(textures(inds,:), true);
    half_texts = textures(1:end/2,:);
    AppearanceModel_half = BuildAppearanceModel(half_texts(inds(1:end/2),:), false);
    
    test_texts = textures(~inds, :);
    for i=1:size(test_texts,1)
        subplot(1,3,1), imshow(AddZerosToImage(mask, test_texts(i,:))/255);
        subplot(1,3,2), imshow(AddZerosToImage(mask, AppearanceParams2Texture(FindModelParameters(AppearanceModel_half, test_texts(i,:)), AppearanceModel_half))/255);
        subplot(1,3,3), imshow(AddZerosToImage(mask, AppearanceParams2Texture(FindModelParameters(AppearanceModel, test_texts(i,:)), AppearanceModel))/255);
        
        subplot(1,2,1), imshow(AddZerosToImage(mask, test_texts(i,:))/255);
        subplot(1,2,2), imshow(AddZerosToImage(mask, AppearanceParams2Texture(FindModelParameters(AppearanceModel, test_texts(i,:)), AppearanceModel))/255);
    end

end

