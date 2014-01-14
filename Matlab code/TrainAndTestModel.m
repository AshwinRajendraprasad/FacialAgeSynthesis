function [ mdl, scale, offset, error ] = TrainAndTestModel( images, modes_inv, gbar, subjectlist, subj_numbers )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
    num_samples = size(images, 1);
    
    % the first half of the dataset is mirrored to give the second half of
    % the dataset, so take random split of the first half only, and then
    % choose the same people mirrored so don't have an image in training
    % and then the mirror in testing 
    indices = randperm(num_samples/2);
    train_ind = [indices(1:end/2),  indices(1:end/2)+num_samples/2];
    test_ind = [indices(end/2+1:end), indices(end/2+1:end)+num_samples/2];
    
    train_images = images(train_ind, :);
    train_numbs = subj_numbers(train_ind);
    test_images = images(test_ind, :);
    test_numbs = subj_numbers(test_ind);
    
    [mdl, scale, offset] = BuildAgeingModel(train_images, modes_inv, gbar, subjectlist, train_numbs);
    
    error = TestModel(mdl, scale, offset, test_images, test_numbs, subjectlist, modes_inv, gbar);

end

