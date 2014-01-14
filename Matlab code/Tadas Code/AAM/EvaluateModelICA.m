function EvaluateModelICA( TrainingData, Model, testDataLocation, seen, unseen, options )
%EVALUATEMODEL Summary of this function goes here
%   Detailed explanation goes here

    %[dispX, dispY] = meshgrid(-30:10:30,-30:10:30);

    dispX = 0;
    dispY = 0;
    
    % Where to output the results
    time = clock;
    timeStr = sprintf('%d-%d-%d-%d-%d', time(1), time(2), time(3), time(4), time(5));
    %outputDir = ['fitted/', timeStr];
    outputDir = ['experiment\\small\\', timeStr];
    mkdir(outputDir);    
    
    fid = fopen([outputDir, '/meta.txt'], 'w');
    for i = 1:numel(options.TrainingDataLocation)
        fprintf(fid, 'Trained on %s\n', options.TrainingDataLocation{i});
    end
    
    fprintf(fid, 'Using ICA for fitting');    
    fprintf(fid, 'Displacing in X from %d, to %d in %d steps\n', min(min(dispX)), max(max(dispX)), size(dispX,2));
    fprintf(fid, 'Displacing in Y from %d, to %d in %d steps\n', min(min(dispY)), max(max(dispY)), size(dispY,1));
    fprintf(fid, 'Color normalisation used - %d\n', Model(1).ColorNorm);
    fprintf(fid, 'PyramidSize - %d\n', numel(Model));
    fprintf(fid, 'First texture dim - %d %d\n', Model(1).AppearanceModel.TextureDimensions(1), Model(1).AppearanceModel.TextureDimensions(1));
    
    fprintf(fid,'\n');
    fclose(fid);
                        
        if(seen)
            
            %% First evaluate the model on convergence on seen data
        
            outputDirSeen = [outputDir '/seen'];
            mkdir(outputDirSeen);    

            % Write out the meta information about the current fitting
            fid = fopen([outputDirSeen, '/meta.txt'], 'w');

            fprintf(fid, 'Fitting to seen data\n');
            fprintf(fid, 'Displacing in X from %d, to %d in %d steps\n', min(min(dispX)), max(max(dispX)), size(dispX,2));
            fprintf(fid, 'Displacing in Y from %d, to %d in %d steps\n', min(min(dispY)), max(max(dispY)), size(dispY,1));
            fprintf(fid, 'Color normalisation used - %d\n', Model(1).ColorNorm);
            fprintf(fid, 'PyramidSize - %d\n', numel(Model));
            fprintf(fid, 'First texture dim - %d %d\n', Model(1).AppearanceModel.TextureDimensions(1), Model(1).AppearanceModel.TextureDimensions(1));
            fclose(fid);
        
            % Evaluate the model by displasing it from optimal translation, but
            % with correct scale and rotation
            %rot = Model(1).ShapeModel.meanRot;
            rot = 0;
            scale = Model(1).ShapeModel.meanScale;

            sx = scale * cos(rot) - 1;
            sy = scale * sin(rot);

            ErrorSeenData = zeros(TrainingData.NumExamples, numel(dispX), numel(Model(1).ShapeModel.MeanShape)/2);
            ErrorAppearanceSeenData = zeros(TrainingData.NumExamples, numel(dispX));
            ErrorPtCSeenData = zeros(TrainingData.NumExamples, numel(dispX));
            Converged = zeros(TrainingData.NumExamples, numel(dispX));        
            Timings = zeros(TrainingData.NumExamples, numel(dispX));
            ConvergedIn = zeros(TrainingData.NumExamples, numel(dispX));
              
            InitP = zeros(numel(Model(end).ShapeModel.Variances),1);
                        
            % Test on all of the seen images
            for i = 3: TrainingData.NumExamples
                % Number of displacements
                
                InputImages(1).I = TrainingData.Images(i).I;
                for k = 2:numel(Model)
                   InputImages(k).I = impyramid (InputImages(k-1).I, 'reduce');
                end                
                
                for j = 1: numel(dispX)

                    tx = mean(TrainingData.xVals(i,:)) + dispX(j);
                    ty = mean(TrainingData.yVals(i,:)) + dispY(j);

                    tic
                       [ T, P, L, numIters ] = ICAFitting(InputImages, Model, [sx,sy,tx,ty], InitP);
                    Timings(i,j) = toc;
                    % should have point to curve estimate (point to point for
                    % now)

                    ConvergedIn(i,j) = numIters;
                    
                    [ErrorPtP, ErrorPtC, ErrorAppearance] = GetFittingError(Model(1), P, L, T, TrainingData.xVals(i,:)', TrainingData.yVals(i,:)', TrainingData.paths, TrainingData.types, InputImages(1).I, options);

                    if(ErrorPtC < 5)
                       Converged(i,j) = 1; 
                    end

                    if(options.OutputFittedImages)                 
                        OutputFittedImages(TrainingData.Images(i).I, Model(1), [sx,sy,tx,ty], T, P, numIters, ErrorPtC, ErrorAppearance,...
                            [outputDirSeen, sprintf( '/fitted%d-%d.png', i, j)]);
                    end

                    ErrorPtCSeenData(i,j) = ErrorPtC;
                    ErrorSeenData(i,j,:) = ErrorPtP;
                    ErrorAppearanceSeenData(i,j) = ErrorAppearance;
                    fprintf('%d,', j);                               
                end

                fprintf('Done seen %d\n', i);            

            end

            save( [outputDir '\ErrorSeen.mat'], 'Converged', 'ConvergedIn', 'Timings', 'dispX','dispY','ErrorPtCSeenData','ErrorSeenData','ErrorAppearanceSeenData');

        end  
        
        if(unseen)
            %% On unseen data but from same database
            dir = {testDataLocation};
            UnseenIMMTestData = ImportShapes(dir, options);

            for i = 1:UnseenIMMTestData.NumExamples   
                UnseenIMMTestData.Images(i).I = Model(1).ConvertTo(UnseenIMMTestData.Images(i).I);    
            end
            
            outputDirUnseen = [outputDir '/unseen'];
            mkdir(outputDirUnseen);    
            
            % Write out the meta information about the current fitting
            fid = fopen([outputDirUnseen, '/meta.txt'], 'w');

            fprintf(fid, 'Fitting to unseen data isomg ICA\n');
            fprintf(fid, 'Displacing in X from %d, to %d in %d steps\n', min(min(dispX)), max(max(dispX)), size(dispX,2));
            fprintf(fid, 'Displacing in Y from %d, to %d in %d steps\n', min(min(dispY)), max(max(dispY)), size(dispY,1));
            fprintf(fid, 'Color normalisation used - %d\n', Model(1).ColorNorm);
            fprintf(fid, 'PyramidSize - %d\n', numel(Model));
            fprintf(fid, 'First texture dim - %d %d\n', Model(1).AppearanceModel.TextureDimensions(1), Model(1).AppearanceModel.TextureDimensions(1));
            fclose(fid);            
            
            ErrorUnseenData = zeros(UnseenIMMTestData.NumExamples, numel(dispX), numel(Model(1).ShapeModel.MeanShape)/2);
            ErrorPtCUnseenData = zeros(UnseenIMMTestData.NumExamples, numel(dispX));
            ConvergedUnseen = zeros(UnseenIMMTestData.NumExamples, numel(dispX));
            ErrorAppearanceUnseenData = zeros(UnseenIMMTestData.NumExamples, numel(dispX));
            Timings = zeros(UnseenIMMTestData.NumExamples, numel(dispX));
            ConvergedIn = zeros(UnseenIMMTestData.NumExamples, numel(dispX));
            
            rot = Model(1).ShapeModel.meanRot;
            scale = Model(1).ShapeModel.meanScale;

            InitP = zeros(numel(Model(end).ShapeModel.Variances),1);            
            
            for i = 1: UnseenIMMTestData.NumExamples

                rot = 0;

                offsetX = mean(UnseenIMMTestData.xVals(i,:));
                offsetY = mean(UnseenIMMTestData.yVals(i,:));

                normX = UnseenIMMTestData.xVals(i,:) - offsetX;
                normY = UnseenIMMTestData.yVals(i,:) - offsetY;

                % Get the Frobenius norm, to scale the shapes to unit size
                scale = norm([normX normY], 'fro');

                sx = scale * cos(rot) - 1;
                sy = scale * sin(rot);                

                InputImages(1).I = UnseenIMMTestData.Images(i).I;
                for k = 2:numel(Model)
                   InputImages(k).I = impyramid (InputImages(k-1).I, 'reduce');
                end                

                for j = 1: numel(dispX)

                    tx = mean(UnseenIMMTestData.xVals(i,:)) + dispX(j);
                    ty = mean(UnseenIMMTestData.yVals(i,:)) + dispY(j);

                    tic                    
                       [ T, P, L, numIters ] = ICAFitting(InputImages, Model, [sx,sy,tx,ty], InitP);
                    Timings(i,j) = toc;
                    % should have point to curve estimate (point to point for
                    % now)

                    ConvergedIn(i,j) = numIters;
                    
                    [ErrorPtP, ErrorPtC, ErrorAppearance] = GetFittingError(Model(1), P, L, T, UnseenIMMTestData.xVals(i,:)', UnseenIMMTestData.yVals(i,:)', UnseenIMMTestData.paths, UnseenIMMTestData.types, InputImages(1).I, options);

                    if(ErrorPtC < 5)
                       ConvergedUnseen(i,j) = 1; 
                    end

                    if(options.OutputFittedImages)                 
                        OutputFittedImages(UnseenIMMTestData.Images(i).I, Model(1), [sx,sy,tx,ty], T, P, numIters, ErrorPtC, ErrorAppearance,...
                            [outputDirUnseen, sprintf( '/fitted%d-%d.png', i, j)]);
                    end

                    ErrorPtCUnseenData(i,j) = ErrorPtC;
                    ErrorUnseenData(i,j,:) = ErrorPtP;
                    ErrorAppearanceUnseenData(i,j) = ErrorAppearance;
                    fprintf('%d,', j);                               
                end                
                
                fprintf('Done unseen %d\n', i);            

            end

            save([outputDir '\ErrorUnseen.mat'], 'ConvergedUnseen','dispX','dispY','ErrorPtCUnseenData','ErrorAppearanceUnseenData', 'ErrorUnseenData', 'ConvergedIn', 'Timings');
        end
end

function [ErrorPtP, ErrorPtC, ErrorAppearance] = GetFittingError(Model, P, L, T, realX, realY, paths, types, Image, options)
    
    Shape = Params2Shape(Model.ShapeModel.PrincipalComponents, P, Model.ShapeModel.MeanShape, T);

    [textureVector, ~] = CreateVectorFromAppearance(Image, Shape, Model.AppearanceModel.ControlPoints, Model.AppearanceModel.TextureDimensions, Model.AppearanceModel.Triangulation, options);

    textureVector = textureVector - mean(textureVector);
    textureVector = textureVector ./ std(textureVector);                

    SynthTexture = Params2Texture(Model.AppearanceModel.PrincipalComponents, L, Model.AppearanceModel.MeanTexture, 0, 1);

    ErrorVector = SynthTexture - textureVector;

    ErrorAppearance = sum(ErrorVector.^2)/numel(ErrorVector);

    ErrorPtP = sqrt((Shape(:,1) - realX).^2 + (Shape(:,2) - realY).^2);
    
    ErrorPtC = PointToCurveDistance(Shape, [realX';realY']', paths, types);
    
end

function OutputFittedImages(Image, Model, TInit, TFinal, P, numIters, ErrorPtC, ErrorAppearance, OutputFile)

    %ShapeI = Params2Shape(Model.ShapeModel.PrincipalComponents, zeros(numel(Model.ShapeModel.Variances),1), Model.ShapeModel.MeanShape, TInit);
    ShapeF = Params2Shape(Model.ShapeModel.PrincipalComponents, P, Model.ShapeModel.MeanShape, TFinal);
    
    Image = Model.ConvertFrom(Image);
    
    %[textureVector, ~] = CreateVectorFromAppearance(Image, Model.AppearanceModel.ControlPoints, ShapeI, Model.AppearanceModel.TextureDimensions, Model.AppearanceModel.Triangulation);
    
    % For adding text on the images     
    %ImageInit = DrawTriangulation(Image, Model.AppearanceModel.Triangulation);
    ImageInit = Image;
    
    [textureVector, ~] = CreateVectorFromAppearance(Image, Model.AppearanceModel.ControlPoints, ShapeF, Model.AppearanceModel.TextureDimensions, Model.AppearanceModel.Triangulation);

    ImageFinal = DrawTriangulation(Image, Model.AppearanceModel.Triangulation);
    
    ImageInit = AddTextToImage(sprintf('Num iters:%d', numIters), ImageInit, 10, 450);
    
    ImageFinal = AddTextToImage(sprintf('App err:%f', ErrorAppearance), ImageFinal, 10, 450);
    ImageFinal = AddTextToImage(sprintf('PtC err:%f',ErrorPtC), ImageFinal, 10, 410);    
    
    catImage = cat(2,ImageInit, ImageFinal);

    imwrite(catImage, OutputFile, 'png');    
    
end