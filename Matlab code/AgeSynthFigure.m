function [ fig, perm, age_est ] = AgeSynthFigure( Model, image, texture, gender, target_ages, mask, M, T, landmarks )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    % Pick random permutation of the three images.  This will be 
    % [orig, younger, older]
    perm = randperm(3);
    
    b = FindModelParameters(Model.App, texture);
    % Return the estimated age to compare to the human estimated age
    age_est = round(PredictAge(Model.AgeEst, b));
    
    % Generate the 3 images
    orig_age_t = AppearanceParams2Texture(b, Model.App);
    orig_age_im = ReverseWarp(AddZerosToImage(mask, orig_age_t, 3), T, landmarks, MtoMean2D(M), image, 3);
    
    younger_age_t = AppearanceParams2Texture(ChangeFaceAge_simple(Model, texture, target_ages(1), gender), Model.App);
    younger_age_im = ReverseWarp(AddZerosToImage(mask, younger_age_t, 3), T, landmarks, MtoMean2D(M), image, 3);
    
    older_age_t = AppearanceParams2Texture(ChangeFaceAge_simple(Model, texture, target_ages(2), gender), Model.App);
    older_age_im = ReverseWarp(AddZerosToImage(mask, older_age_t, 3), T, landmarks, MtoMean2D(M), image, 3);
    
    fig = figure;
    subplot(1,3,perm(1)), imshow(orig_age_im);
    subplot(1,3,perm(2)), imshow(younger_age_im);
    subplot(1,3,perm(3)), imshow(older_age_im);

end

