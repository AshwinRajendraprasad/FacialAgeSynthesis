function [ AgeingModel, diff ] = TrainAndTestAgeingModel_reg( textures, AppearanceModel, subj_numbers, subjectlist, lambda )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

    all_numbers = unique(subj_numbers);
    % Split the subjects into 2/3 for training and 1/3 for testing
    indices = randperm(size(all_numbers,1));
    train_subj_inds = indices(1:round(end/3*2));
    test_subj_inds = indices(round(end/3*2)+1:end);
    
    train_subjects = all_numbers(train_subj_inds);
    test_subjects = all_numbers(test_subj_inds);
    
    % Create logical array of the textures in each set
    train_tex_inds = false(size(subj_numbers));
    test_tex_inds = false(size(subj_numbers));
    
    for i=1:size(train_subjects)
        train_tex_inds = train_tex_inds | subj_numbers==train_subjects(i);
    end
    
    for i=1:size(test_subjects)
        test_tex_inds = test_tex_inds | subj_numbers==test_subjects(i);
    end
    
    train_textures = textures(train_tex_inds,:);
    test_textures = textures(test_tex_inds,:);
    
    AppearanceModel = BuildAppearanceModel(train_textures, false);
    
    if lambda==0
        AgeingModel = BuildAgeingModel_reg(train_textures, AppearanceModel, subjectlist, subj_numbers(train_tex_inds));
        diff = TestAgeingModel_reg(AgeingModel, AppearanceModel, test_textures, subj_numbers(test_tex_inds), subjectlist);
    else
        AgeingModel = BuildAgeingModel_reg_singlelam(train_textures, AppearanceModel, subjectlist, subj_numbers(train_tex_inds), lambda);
        diff = TestAgeingModel(AgeingModel, AppearanceModel, test_textures, subj_numbers(test_tex_inds), subjectlist);
    end

end

