function [ train, test ] = SplitDataset_traintest( textures, subject_numbers )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    all_numbers = unique(subject_numbers);
    % Split the subjects into 2/3 for training and 1/3 for testing
    indices = randperm(size(all_numbers,1));
    train_subj_inds = indices(1:round(end/3*2));
    test_subj_inds = indices(round(end/3*2)+1:end);
    
    train_subjects = all_numbers(train_subj_inds);
    test_subjects = all_numbers(test_subj_inds);
    
    % Create logical array of the textures in each set
    train_tex_inds = false(size(subject_numbers));
    test_tex_inds = false(size(subject_numbers));
    
    for i=1:size(train_subjects)
        train_tex_inds = train_tex_inds | subject_numbers==train_subjects(i);
    end
    
    for i=1:size(test_subjects)
        test_tex_inds = test_tex_inds | subject_numbers==test_subjects(i);
    end
    
    % split the textures and store in correct struct
    train.textures = textures(train_tex_inds,:);
    test.textures = textures(test_tex_inds,:);
    
    % split subject numbers and store in correct struct
    train.numbers = subject_numbers(train_tex_inds);
    test.numbers = subject_numbers(test_tex_inds);
end

