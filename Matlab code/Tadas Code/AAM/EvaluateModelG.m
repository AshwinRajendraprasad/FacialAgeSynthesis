function EvaluateModelG(  TrainingData, Model, testDataLocation, seen, unseen, unseenDB, options )
%EVALUATEMODELG Summary of this function goes here
%   Detailed explanation goes here

    % Where to output the results
    time = clock;
    timeStr = sprintf('%d-%d-%d-%d-%d', time(1), time(2), time(3), time(4), time(5));
    %outputDir = ['fitted/', timeStr];
    outputDir = ['experiment\\globalFit\\', timeStr];
    mkdir(outputDir);    
    
    fid = fopen([outputDir, '/meta.txt'], 'w');
    for i = 1:numel(options.TrainingDataLocation)
        fprintf(fid, 'Trained on %s\n', options.TrainingDataLocation{i});

    end
    
    fprintf(fid, 'Color normalisation used - %d\n', Model(1).ColorNorm);
    fprintf(fid, 'PyramidSize - %d\n', numel(Model));
    fprintf(fid, 'First texture dim - %d %d\n', Model(1).AppearanceModel.TextureDimensions(1), Model(1).AppearanceModel.TextureDimensions(1));
    fprintf(fid, 'Best fit used - %d\n', options.CootesBestFit);
    
    fprintf(fid,'\n');
    fclose(fid);
    
    % use the fixed jacobian model
    if(strcmpi(Model(1).Type,'FJ'))
                    
        if(seen)
            % First evaluate the model on convergence on seen data
        
            outputDirSeen = [outputDir '/seen'];
            mkdir(outputDirSeen);    

            % Write out the meta information about the current fitting
            fid = fopen([outputDirSeen, '/meta.txt'], 'w');

            fprintf(fid, 'Fitting to seen data\n');
            fprintf(fid, 'Color normalisation used - %d\n', Model(1).ColorNorm);
            fprintf(fid, 'PyramidSize - %d\n', numel(Model));
            fprintf(fid, 'First texture dim - %d %d\n', Model(1).AppearanceModel.TextureDimensions(1), Model(1).AppearanceModel.TextureDimensions(1));
            fprintf(fid, 'Best fit used - %d\n', options.CootesBestFit);
            fclose(fid);
        
            ErrorPtP = zeros(TrainingData.NumExamples,1);
            ErrorAppearance = zeros(TrainingData.NumExamples, 1);
            ErrorPtC = zeros(TrainingData.NumExamples, 1);
            Converged = zeros(TrainingData.NumExamples, 1);        
            Timings = zeros(TrainingData.NumExamples, 1);
            ConvergedIn = zeros(TrainingData.NumExamples, 1);
            
            % Record the new params so that to reproduce the fitting
            Ts = zeros(TrainingData.NumExamples, 4);
            Cs = zeros(TrainingData.NumExamples, numel(Model(1).CombinedModel.Variances));
            
            % Test on all of the seen images
            for i = 1: TrainingData.NumExamples
                
                tic
                    [Cn, Tn, options.Recording, numIters, ErrorAppearance(i)] = GlobalFittingCootes(TrainingData.Images(i).I, Model, options);
                Timings(i) = toc;
                
                ConvergedIn(i) = numIters;

                [ErrorPtP(i), ErrorPtC(i)] = GetFittingError(Model(1), Cn, Tn, TrainingData.xVals(i,:)', TrainingData.yVals(i,:)', TrainingData.paths, TrainingData.types, TrainingData.Images(i).I,options);

                if(ErrorPtC(i) < 5)
                   Converged(i) = 1; 
                end

                Ts(i, :) = Tn;
                Cs(i, :) = Cn;

                if(options.OutputFittedImages)                 
                    OutputFittedImages(TrainingData.Images(i).I, Model(1), Cn, Tn, ErrorPtP(i), ErrorPtC(i), ErrorAppearance(i),...
                        [outputDirSeen, sprintf( '/fitted%d.png', i)]);
                end

                fprintf('Done seen %d\n', i);            

            end
            save( [outputDir '\ErrorSeen.mat'], 'Converged', 'ConvergedIn', 'Timings', 'ErrorPtC','ErrorAppearance','ErrorPtP');
            save( [outputDir '\FittingSeen.mat'], 'Ts','Cs');
        end  
        
        if(unseen)
            %% On unseen data but from same database
            dir = {testDataLocation};
            UnseenTestData = ImportShapes(dir, options);

            for i = 1:UnseenTestData.NumExamples
   
                UnseenTestData.Images(i).I = Model(1).ConvertTo(UnseenTestData.Images(i).I);
    
            end
            
            outputDirUnseen = [outputDir '/unseen'];
            mkdir(outputDirUnseen);    
            
            % Write out the meta information about the current fitting
            fid = fopen([outputDirUnseen, '/meta.txt'], 'w');

            fprintf(fid, 'Fitting to unseen data\n');
            fprintf(fid, 'Color normalisation used - %d\n', Model(1).ColorNorm);
            fprintf(fid, 'PyramidSize - %d\n', numel(Model));
            fprintf(fid, 'First texture dim - %d %d\n', Model(1).AppearanceModel.TextureDimensions(1), Model(1).AppearanceModel.TextureDimensions(1));
            fprintf(fid, 'Best fit used - %d\n', options.CootesBestFit);
            fclose(fid);            
            
            ErrorPtP = zeros(UnseenTestData.NumExamples,1);
            ErrorAppearance = zeros(UnseenTestData.NumExamples, 1);
            ErrorPtC = zeros(UnseenTestData.NumExamples, 1);
            Converged = zeros(UnseenTestData.NumExamples, 1);        
            Timings = zeros(UnseenTestData.NumExamples, 1);
            ConvergedIn = zeros(UnseenTestData.NumExamples, 1);
            
            % Record the new params so that to reproduce the fitting
            Ts = zeros(UnseenTestData.NumExamples, 4);
            Cs = zeros(UnseenTestData.NumExamples, numel(Model(1).CombinedModel.Variances));
            
            % Test on all of the seen images
            for i = 1: UnseenTestData.NumExamples
                
                tic
                    [Cn, Tn, options.Recording, numIters, ErrorAppearance(i)] = GlobalFittingCootes(UnseenTestData.Images(i).I, Model, options);
                Timings(i) = toc;
                
                ConvergedIn(i) = numIters;

                [ErrorPtP(i), ErrorPtC(i), ~] = GetFittingError(Model(1), Cn, Tn, UnseenTestData.xVals(i,:)', UnseenTestData.yVals(i,:)', UnseenTestData.paths, UnseenTestData.types, UnseenTestData.Images(i).I,options);

                if(ErrorPtC(i) < 5)
                   Converged(i) = 1; 
                end

                Ts(i, :) = Tn;
                Cs(i, :) = Cn;

                if(options.OutputFittedImages)                 
                    OutputFittedImages(UnseenTestData.Images(i).I, Model(1), Cn, Tn, ErrorPtP(i), ErrorPtC(i), ErrorAppearance(i),...
                        [outputDirUnseen, sprintf( '/fitted%d.png', i)]);
                end

                fprintf('Done seen %d\n', i);            

            end
            
            save([outputDir '\ErrorUnseen.mat'], 'Converged', 'ConvergedIn', 'Timings', 'ErrorPtC','ErrorAppearance','ErrorPtP');
            save( [outputDir '\FittingUnseen.mat'], 'Ts','Cs');
        end
        
        %% On unseen data on a different database
        if(unseenDB)

        end
    end

end

function [ErrorPtP, ErrorPtC, ErrorAppearance] = GetFittingError(Model, C, T, realX, realY, paths, types, Image, options)
    
    % Evaluate how good the match is
    B = Model.CombinedModel.PrincipalComponents * C;
    Bs = inv(Model.CombinedModel.Ws) * B(1:numel(Model.ShapeModel.Variances));          
    Bg = B(numel(Model.ShapeModel.Variances)+1:end);          

    Shape = Params2Shape(Model.ShapeModel.PrincipalComponents, Bs, Model.ShapeModel.MeanShape, T);

    [textureVector, ~] = CreateVectorFromAppearance(Image, Shape, Model.AppearanceModel.ControlPoints, Model.AppearanceModel.TextureDimensions, Model.AppearanceModel.Triangulation,options);

    textureVector = textureVector - mean(textureVector);
    textureVector = textureVector ./ std(textureVector);                

    SynthTexture = Params2Texture(Model.AppearanceModel.PrincipalComponents, Bg, Model.AppearanceModel.MeanTexture, 0, 1);

    ErrorVector = SynthTexture - textureVector;

    ErrorAppearance = sum(ErrorVector.^2)/numel(ErrorVector);                

    ErrorPtP = mean(sqrt((Shape(:,1) - realX).^2 + (Shape(:,2) - realY).^2)); 
    
    ErrorPtC = PointToCurveDistance(Shape, [realX';realY']', paths, types);
    
end

function OutputFittedImages(Image, Model, C, TFinal, ErrorPtP, ErrorPtC, ErrorAppearance,OutputFile)

    B = Model.CombinedModel.PrincipalComponents * C;
    Bs = inv(Model.CombinedModel.Ws) * B(1:numel(Model.ShapeModel.Variances));          
    Bg = B(numel(Model.ShapeModel.Variances)+1:end);          

    % For adding text on the images                   
    FittedImage = DrawTextureOnTop(Image, Model, Bs, Bg, TFinal, Model.AppearanceModel.Transform.TranslateGlobal, Model.AppearanceModel.Transform.ScaleGlobal);
    FittedImage = Model.ConvertFrom(FittedImage);
    FittedImage = AddTextToImage(sprintf('App err:%f', ErrorAppearance), FittedImage, 10, 450);
    FittedImage = AddTextToImage(sprintf('PtC err:%f',ErrorPtC), FittedImage, 10, 410);    
    FittedImage = AddTextToImage(sprintf('PtP err:%f',ErrorPtP), FittedImage, 10, 370);
    imwrite(FittedImage, OutputFile, 'png');    

end
