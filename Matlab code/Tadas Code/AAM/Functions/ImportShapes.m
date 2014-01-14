function [TrainingData] = ImportShapes( directories, options )
%IMPORTSHAPES imports annotattations (.asf) and images for
%   Provide the directory of the training data
    
    TrainingData.NumExamples = 0;    
    TrainingData.fileName = {};
        
    TrainingData.xVals = [];
    TrainingData.yVals = [];
    TrainingData.Images = [];
    
    for d = 1:numel(directories)

        directory = directories{d};

        % get all the .asf files from this directory
        files = dir([directory  '/*.asf']);

        numFiles = numel(files);        
                
        xVals = [];
        yVals = [];        

        Images = struct;
        
        % iterate over all the files and read the data
        for i=1:numFiles

            % open the file
            currentFid = fopen([directory '/' files(i).name]);
            TrainingData.fileName{TrainingData.NumExamples + i} = files(i).name;
            y = 0;                
            tline = fgetl(currentFid);

            % Read the file
            while ischar(tline)
                % skip empty lines
                if(size(tline) ~= 0)
                    if((size(regexp(tline, '\w*#')) == 0))
                        if(y == 0)
                            numPoints = str2double(tline);
                            path = zeros(numPoints,1);
                            type = zeros(numPoints,1);
                            xRel = zeros(numPoints,1);
                            yRel = zeros(numPoints,1);
                            point = zeros(numPoints,1);
                            connectFrom = zeros(numPoints,1);
                            connectTo = zeros(numPoints,1);
                            y = y+1;
                            % if its the first file pre-allocate the output
                            if(i == 1)

                                % The only things to differ over the training
                                % examples
                                xVals = zeros(numFiles, numPoints);
                                yVals = zeros(numFiles, numPoints);

                            end
                        elseif (y <= numPoints)
                            data = str2num(tline);
                            path(y) = data(1);
                            type(y) = data(2);
                            xRel(y) = data(3);
                            yRel(y) = data(4);
                            point(y) = data(5);
                            connectFrom(y) = data(6);
                            connectTo(y) = data(7);                        
                            y = y+1;
                        else
                           imageName = tline; 
                        end
                    end
                end
                tline = fgetl(currentFid);
            end                

            fclose(currentFid);
            
            %read the image to transform from rel to abs coords
            if (TrainingData.NumExamples + i) == 1

                TrainingData.paths = path;
                TrainingData.types = type;

                TrainingData.points = point;
                TrainingData.connectFroms = connectFrom;
                TrainingData.connectTos = connectTo;

            end

            Images(i).I(:,:,:) = im2double(imread([directory '/' imageName]));

            [M,N,~] = size(Images(i).I);

            % transform to texture locations (Texture Sizes could differ)
            xVals(i,:) = xRel * options.ImportDim(1);
            yVals(i,:) = yRel * options.ImportDim(2);
            
            Images(i).I = ResizeImage(Images(i).I, options.ImportDim(2), options.ImportDim(1));
            
            % Diplay the image that was read
            if(options.verbose)

                imshow(DrawObject(xVals(TrainingData.NumExamples + i,:), yVals(TrainingData.NumExamples + i,:), path, type, Images(i).I));
                drawnow('expose')

            end

        end
        
        if(numFiles > 0)
            TrainingData.xVals = cat(1, TrainingData.xVals, xVals);
            TrainingData.yVals = cat(1, TrainingData.yVals, yVals);
            TrainingData.Images = [TrainingData.Images; Images'];
        else                    
            fprintf('No files found to read in dir %s\n', directory);
        end
        
        TrainingData.NumExamples = TrainingData.NumExamples + numFiles;
    
    end    

end

