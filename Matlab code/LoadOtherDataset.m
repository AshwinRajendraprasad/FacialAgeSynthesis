cd('C:\dataset\face_data\data');

files = dir('*.jpg');
images = zeros(length(files), 480, 640, 3);
for i=1:length(files)
    images(i,:,:,:) = importdata(files(i).name);
end

cd('.\landmarks');

files = dir('*.pts');
landmarks = zeros(length(files), 66, 2);
for i=1:length(files)
    landmarks(i,:,:) = ImportLandmarks(files(i).name);
end