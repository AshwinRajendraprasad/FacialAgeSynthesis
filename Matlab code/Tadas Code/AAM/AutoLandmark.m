function AutoLandmark
%AUTOLANDMARK Given an existing statistical model tries to automatically
%landmark the unseen images and output the .asf files
%   Detailed explanation goes here

LoadDependencies;

options = struct;
options.DisplayShapeVariations = false;
options.DisplayAppearanceVariations = false;
options.DisplayCombinedVariations = true;

options.NumComponentsToShow = 6;
options.NumParamsToShow = 5;

options.verbose = false;

options.TangentSpaceTransform = true;
options.IlluminationCorrection = true;

% Warp type
% Interpolation type
% Colour settings

options.PreserveShapeVariation = 0.95;
options.PreserveAppearanceVariation = 0.95;
options.PreserveCombinedVariation = 0.95;

options.CootesBestFit = false;

options.MaxIterationsCootes = 30;

options.ColorNorm = true;

options.TextureSize = [100 100];
options.Interpolation = 'BI';

options.PyramidSize = 3;

%options.TrainingDataLocation = {'TrainingData\imm\frontal', 'TrainingData\multiPie\frontalLabelled\b1'};
options.TrainingDataLocation = {'Data\\imm new\\all', 'Data\\multiPie\\frontal\\labeled'};
%options.TrainingDataLocation = {'Data\\imm new\\all'};
options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMAllPieSome-Pyr3_100';
options.LoadTrainingData = true;
options.DataToLoad = 'IMMAllPieSome-Pyr3_100.mat';

options.RecordFitting = false;
%options.Recording = avifile('fitting.avi','compression','none','fps',5); %creates AVI file, test.avi 
%options.hf= figure('visible','off'); %turns visibility of figure off 
%options.hax=axes; 

options.OutputFittedImages = true;

options.ColourScheme = 'I1I2I3';
options.BandNormalisation = 'Channel';

% Using the inferred parameters from previous pyramid steps, or just scale
% rotation and orientation (compare results numerically).

%% Training the AAM model (or loading the previously trained and saved one)
[TrainingData, Model] = AAMTraining(options);

%% Attempt to fit at various x and y locations   

directory = 'Data\\multiPie\\frontal\b9';
FitAndOutput(TrainingData, Model, directory, options);
directory = 'Data\\multiPie\\frontal\b10';
FitAndOutput(TrainingData, Model, directory, options);
directory = 'Data\\multiPie\\frontal\b11';
FitAndOutput(TrainingData, Model, directory, options);
directory = 'Data\\multiPie\\frontal\b12';
FitAndOutput(TrainingData, Model, directory, options);
directory = 'Data\\multiPie\\frontal\b13';
FitAndOutput(TrainingData, Model, directory, options);
directory = 'Data\\multiPie\\frontal\b14';
FitAndOutput(TrainingData, Model, directory, options);

end