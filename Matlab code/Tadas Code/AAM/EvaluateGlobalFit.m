function [ output_args ] = EvaluateGlobalFit( input_args )
%EVALUATEGLOBALFIT Summary of this function goes here
%   Detailed explanation goes here

%%

LoadDependencies;

options = struct;
options.DisplayShapeVariations = true;
options.DisplayAppearanceVariations = true;
options.DisplayCombinedVariations = true;

options.NumComponentsToShow = 6;
options.NumParamsToShow = 5;

options.verbose = false;

options.TangentSpaceTransform = true;
options.IlluminationCorrection = true;

% Warp type
% Interpolation type
% Colour settings

options.PreserveShapeVariation = 0.98;
options.PreserveAppearanceVariation = 0.98;
options.PreserveCombinedVariation = 0.98;

options.CootesBestFit = false;

options.MaxIterationsCootes = 30;

options.ColorNorm = true;

options.TextureSize = [200 200];
options.Interpolation = 'BI';

options.PyramidSize = 3;

options.RecordFitting = false;

%options.Recording = avifile('fitting.avi','compression','none','fps',5); %creates AVI file, test.avi 
%options.hf= figure('visible','off'); %turns visibility of figure off 
%options.hax=axes; 
options.OutputFittedImages = true;

%% Colour spec
options.ColourScheme = 'I1I2I3';
options.BandNormalisation = 'Channel';

%% Training on Pie and IMM and testing on both
%options.TrainingDataLocation = {'Data\\imm new\\all', 'Data\\multiPie\\frontal\\labeled'};
options.TrainingDataLocation = {'Data\\imm new\\all'};
options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMAll-Pyr3_100';
options.LoadTrainingData = false;
options.DataToLoad = 'IMMAll-Pyr3_100.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModelG(TrainingData, Model, 'Data\imm\frontal\2', true, false, false, options);
%EvaluateModelG(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
%EvaluateModelG(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

end

