function [meanError, rmsError, errorVariance, meanErrors, all_errors] = calcBiwiError(resDir, gtDir)

seqNames = {'01','02','03','04','05','06','07','08','09', ...
    '10', '11','12','13','14','15','16','17','18','19', ...
    '20', '21','22','23','24'};

rotMeanErr = zeros(numel(seqNames),3);
rotRMS = zeros(numel(seqNames),3);
rot = cell(1,numel(seqNames));
rotg = cell(1,numel(seqNames));

tic;
for i=1:numel(seqNames)
        
    posesGround =  load ([gtDir '/' seqNames{i} '/groundTruthPose.txt']);
    rotg{i} = posesGround(:,[5 6 7])  * 180 / pi;
    
    [frame time sc tx ty tz rx ry rz] = textread([resDir '/' seqNames{i} '.txt'], '%f %f %f %f %f %f %f %f %f');
  
    rot{i} = [rx ry rz] * 180 / pi;

    rot{i}(:,1) = rot{i}(:,1) - rot{i}(1,1);
    rot{i}(:,2) = rot{i}(:,2) - rot{i}(1,2);
    rot{i}(:,3) = rot{i}(:,3) - rot{i}(1,3);        

    rotg{i}(:,1) = rotg{i}(:,1) - rotg{i}(1,1);
    rotg{i}(:,2) = rotg{i}(:,2) - rotg{i}(1,2);
    rotg{i}(:,3) = rotg{i}(:,3) - rotg{i}(1,3);        

    rotMeanErr(i,:) = mean(abs((rot{i}(:,:)-rotg{i}(:,:))));
    rotRMS(i,:) = sqrt(mean(((rot{i}(:,:)-rotg{i}(:,:))).^2)); 
end
%%
meanErrors = rotMeanErr;
allRot = cell2mat(rot');
allRotg = cell2mat(rotg');
meanError = mean(abs((allRot(:,:)-allRotg(:,:))));

all_errors = abs(allRot-allRotg);

rmsError = sqrt(mean(((allRot(:,:)-allRotg(:,:))).^2)); 
errorVariance = std(abs((allRot(:,:)-allRotg(:,:))));