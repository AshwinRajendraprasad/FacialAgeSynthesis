function [TrainingData, StatisticalModel] = AAMTraining(options)

%% Loading the image data

% Setting up the colour transforms
if( isfield(options, 'ColourScheme') )
    [ConvertTo, ConvertFrom, numberBands] = GetColourTransform(options.ColourScheme);
else
    fprintf('Warning no colour scheme was specified \n');
    [ConvertTo, ConvertFrom, numberBands] = GetColourTransform('RGB');
end
    
% Can either be small, colour, or all (IMM database for now)
TrainingData = ImportShapes(options.TrainingDataLocation, options);

% Convert the read images to the desired format
for i = 1:TrainingData.NumExamples
   
   TrainingData.Images(i).I = ConvertTo(TrainingData.Images(i).I);
    
end

%% either load the statistical model or recalculate it
if(options.LoadTrainingData)

    load(options.DataToLoad);
    
%     % Make sure the colour scheme from the loaded model matches that of the
%     % model in options
    if(isfield(StatisticalModel(1), 'ColourScheme'))        
        if(strcmpi(StatisticalModel(1).ColourScheme, options.ColourScheme))
            StatisticalModel(1).ConvertTo = ConvertTo;
            StatisticalModel(1).ConvertFrom = ConvertFrom;
            StatisticalModel(1).NumberBands = numberBands;
        else
            fprintf('Colour scheme mismatch\n');
        end
    else
        fprintf('No colour scheme found, options specified is used\n');
        StatisticalModel(1).ConvertTo = ConvertTo;
        StatisticalModel(1).ConvertFrom = ConvertFrom;
        StatisticalModel(1).NumberBands = numberBands;
    end
else
    %% Creating the statistical model of shape and appearance
    StatisticalModel = struct;

    %% Go over different scales
    for i = 1:options.PyramidSize
        
        StatisticalModel(i).ColourScheme = options.ColourScheme;
        StatisticalModel(i).Type = 'FJ';
    
        StatisticalModel(i).ConvertTo = ConvertTo;
        StatisticalModel(i).ConvertFrom = ConvertFrom;
        StatisticalModel(i).NumberBands = numberBands;
         
        options.ConvertTo = ConvertTo;
        options.ConvertFrom = ConvertFrom;
        
        %% Creating a shape model
        
        StatisticalModel(i).ShapeModel = CreateShapeModel(TrainingData, options);

        StatisticalModel(i).ShapeModel.paths = TrainingData.paths;
        StatisticalModel(i).ShapeModel.types = TrainingData.types;        
        
        %% Creating an appearance model
        StatisticalModel(i).AppearanceModel = CreateAppearanceModel(TrainingData, StatisticalModel(i).ShapeModel, options);                 
        
        %% Displaying the variations
        if(options.DisplayShapeVariations)
            DisplayVariationsShape(StatisticalModel(i), options);
        end
        
        if(options.DisplayAppearanceVariations)
            % Display the variations in the appearance
            DisplayVariationsAppearance(StatisticalModel(i), options);
        end
       
        
        %% Creating a combined model
        StatisticalModel(i).CombinedModel = CreateCombinedModel(StatisticalModel(i), options);

        if(options.DisplayCombinedVariations)
            % Display the variations in the appearance
            DisplayCombinedVariations(StatisticalModel(i), options);
        end
        
        
        %% Pretrain the model (currently using the basic approach proposed by Cootes
        StatisticalModel(i).ColorNorm = options.ColorNorm;
        [StatisticalModel(i).R, StatisticalModel(i).T] = PretrainAAM(TrainingData, StatisticalModel(i), options);

        % Rescale the training data if going down a level
        if( i < options.PyramidSize )
        
            for j = 1:TrainingData.NumExamples
                TrainingData.Images(j).I = impyramid(TrainingData.Images(j).I, 'reduce');
            end
            
            TrainingData.xVals = TrainingData.xVals ./ 2;
            TrainingData.yVals = TrainingData.yVals ./ 2;
            
            options.TextureSize = options.TextureSize ./ 2;
            
        end

        %% Clear the unneeded data from the model
        StatisticalModel(i).ShapeModel.NormalisedShapes = [];
        StatisticalModel(i).ShapeModel.Transform = [];
        StatisticalModel(i).AppearanceModel.TextureVectors = [];        
        
    end
    % Reload the training data
    TrainingData = ImportShapes(options.TrainingDataLocation, options);
    if(options.SaveTraining)
        save(options.SaveTrainingLoc, 'StatisticalModel');
    end
end

end