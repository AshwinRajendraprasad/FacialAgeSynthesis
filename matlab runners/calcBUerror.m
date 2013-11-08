function [meanError, rmsError, errorVariance, all_errors] = calcBUerror(resDir, gtDir)

seqNames = {'jam1','jam2','jam3','jam4','jam5','jam6','jam7','jam8','jam9', ...
    'jim1','jim2','jim3','jim4','jim5','jim6','jim7','jim8','jim9', ...
    'llm1','llm2','llm3','llm4','llm5','llm6','llm7','llm8','llm9', ...
    'ssm1','ssm2','ssm3','ssm4','ssm5','ssm6','ssm7','ssm8','ssm9', ...
    'vam1','vam2','vam3','vam4','vam5','vam6','vam7','vam8','vam9'};

rotMeanErr = zeros(numel(seqNames),3);
rotRMS = zeros(numel(seqNames),3);
tMeanErr = zeros(numel(seqNames),3);
tRMS = zeros(numel(seqNames),3);
rot = cell(1,numel(seqNames));
rotg = cell(1,numel(seqNames));
tg = cell(1,numel(seqNames));

for i = 1:numel(seqNames)
    
    [frame time sc tx ty tz rx ry rz] = textread([resDir seqNames{i} '.txt'], '%f %f %f %f %f %f %f %f %f');
    posesGround =  load ([gtDir seqNames{i} '.dat']);
    rot{i} = [rx ry rz]*180/pi;

    % as BU assumes initial frontal and Gavam + clm doesn't
    rot{i}(:,1) = rot{i}(:,1) - rot{i}(1,1);
    rot{i}(:,2) = rot{i}(:,2) - rot{i}(1,2);
    rot{i}(:,3) = rot{i}(:,3) - rot{i}(1,3);  
    
    t{i} = [tx ty tz]/25.4;
    t{i}(:,1) = (t{i}(:,1) - t{i}(1,1));
    t{i}(:,2) = t{i}(:,2) - t{i}(1,2);
    t{i}(:,3) = (t{i}(:,3) - t{i}(1,3));    

    rotg{i} = posesGround(2:end,[7 6 5]);
    tg{i} = posesGround(2:end,[2 3 4]);
    tg{i}(:,1) = tg{i}(:,1) - tg{i}(1,1);
    tg{i}(:,2) = tg{i}(:,2) - tg{i}(1,2);
    tg{i}(:,3) = tg{i}(:,3) - tg{i}(1,3);
    rotg{i} = rotg{i};
    
    rotg{i}(:,1) = rotg{i}(:,1) - rotg{i}(1,1);
    rotg{i}(:,2) = rotg{i}(:,2) - rotg{i}(1,2);
    rotg{i}(:,3) = rotg{i}(:,3) - rotg{i}(1,3);  
        
    rot{i}(:,2) = -rot{i}(:,2);
    rot{i}(:,3) = -rot{i}(:,3);
    t{i}(:,1) = -t{i}(:,1);
    
    rotMeanErr(i,:) = mean(abs((rot{i}(:,:)-rotg{i}(:,:))));
    rotRMS(i,:) = sqrt(mean(((rot{i}(:,:)-rotg{i}(:,:))).^2)); 
    tMeanErr(i,:) = mean(abs(t{i}(:,:)-tg{i}(:,:)));
    tRMS(i,:) = sqrt(mean((t{i}(:,:)-tg{i}(:,:)).^2)); 
end
allRot = cell2mat(rot');
allRotg = cell2mat(rotg');

meanError = mean(abs((allRot(:,:)-allRotg(:,:))));
all_errors = abs(allRot-allRotg);
rmsError = sqrt(mean(((allRot(:,:)-allRotg(:,:))).^2)); 
errorVariance = var(abs((allRot(:,:)-allRotg(:,:))));      