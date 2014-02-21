function [images, landmarks, ages, genders] = LoadDataset(path)
%LoadDataset Loads images and landmarks from path
%   Assumes images are in path, landmarks in path\landmarks
    origPath = pwd;
    cd(path);

    % Load images
    files = dir('*.jpg');
%     images = zeros(length(files), dimx, dimy, numChannels);
    images(length(files)).image = 0;
    ages = zeros(length(files),1);
    genders = cell(length(files),1);
    for i=1:length(files)
        images(i).image = importdata(files(i).name);
        % the first digit is the beginning of the age, 2 digits long
        ind = regexp(files(i).name, '\d*');
        ages(i) = str2double(files(i).name(ind:ind+1));
        % the first lower case letter is for the gender, use it to signify
        % the gender (m or f)
        ind = regexp(files(i).name, '[a-z]');
        genders{i} = files(i).name(ind(1));
    end
    % transpose so it fits with rest of code better (number of images as
    % 1st dimension
    images = images';

    % Load landmarks
    cd('.\landmarks');

    files = dir('*.pts');
    landmarks = zeros(length(files), 66, 2);
    for i=1:length(files)
        landmarks(i,:,:) = ImportLandmarks(files(i).name);
    end
    
    cd(origPath);
end