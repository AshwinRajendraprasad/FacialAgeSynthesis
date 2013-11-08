function [meanError, rmsError, errorVariance, meanErrors, all_errors] = calcIctError(resDir, gtDir)
%CALCICTERROR Summary of this function goes here
%   Detailed explanation goes here

    polhemus = 'polhemusNorm.csv';

    sequences = dir([resDir '*.txt']);

    rotMeanErr = zeros(numel(sequences),3);
    rotRMS = zeros(numel(sequences),3);
    rot = cell(1,numel(sequences));
    rotg = cell(1,numel(sequences));

    for i = 1:numel(sequences)

        [~, name,~] = fileparts(sequences(i).name);
        [frame time sc tx ty tz rx ry rz] = textread([resDir '/' sequences(i).name], '%f %f %f %f %f %f %f %f %f');
        [txg tyg tzg rxg ryg rzg] =  textread([gtDir name '/'  polhemus], '%f,%f,%f,%f,%f,%f');

        rot{i} = [rx ry rz] * (180/ pi);

        rotg{i} = [rxg ryg rzg] * (180/ pi);

        rot{i}(:,1) = rot{i}(:,1) - rot{i}(1,1);
        rot{i}(:,2) = rot{i}(:,2) - rot{i}(1,2);
        rot{i}(:,3) = rot{i}(:,3) - rot{i}(1,3);        

        rotMeanErr(i,:) = mean(abs((rot{i}(:,:)-rotg{i}(:,:))));
        rotRMS(i,:) = sqrt(mean(((rot{i}(:,:)-rotg{i}(:,:))).^2)); 
    end
    allRot = cell2mat(rot');
    allRotg = cell2mat(rotg');
    meanErrors = rotMeanErr;
    meanError = mean(abs((allRot(:,:)-allRotg(:,:))));
    all_errors = abs(allRot-allRotg);
    rmsError = sqrt(mean(((allRot(:,:)-allRotg(:,:))).^2)); 
    errorVariance = var(abs((allRot(:,:)-allRotg(:,:))));  
end

