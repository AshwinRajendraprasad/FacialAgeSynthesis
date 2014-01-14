function EvaluateAAMs
%EVALUATEAAMS Summary of this function goes here
%   Detailed explanation goes here

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
options.ImportDim = [640 480];

%options.Recording = avifile('fitting.avi','compression','none','fps',5); %creates AVI file, test.avi 
%options.hf= figure('visible','off'); %turns visibility of figure off 
%options.hax=axes; 
options.OutputFittedImages = true;

%% Colour spec
options.ColourScheme = 'RGB';
options.BandNormalisation = 'Global';

% Using the inferred parameters from previous pyramid steps, or just scale
% rotation and orientation (compare results numerically).

%% Training on IMM and testing on both
options.TrainingDataLocation = {'Data\imm\frontal\1'};

options.SaveTraining = true;
options.SaveTrainingLoc = 'IMMFrontal1RGB-G-Pyr3_200';

options.LoadTrainingData = false;
options.DataToLoad = 'IMMFrontal1RGB-G-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

%% Training on Pie and testing on both
options.TrainingDataLocation = {'Data\multiPie\experiment\fitting\a1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'PieFrontal1RGB-G-Pyr3_200';

options.LoadTrainingData = true;
options.DataToLoad = 'PieFrontal1RGB-G-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

%% Training on Pie and IMM and testing on both
options.TrainingDataLocation = {'Data\imm\frontal\1', 'Data\multiPie\experiment\fitting\a1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMPieFrontal1RGB-G-Pyr3_200';

options.LoadTrainingData = true;
options.DataToLoad = 'IMMPieFrontal1RGB-G-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

%% Colour spec
options.ColourScheme = 'Grayscale';
options.BandNormalisation = 'Global';

% Using the inferred parameters from previous pyramid steps, or just scale
% rotation and orientation (compare results numerically).

%% Training on IMM and testing on both
options.TrainingDataLocation = {'Data\imm\frontal\1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMFrontal1Gray-G-Pyr3_200';

options.LoadTrainingData = true;
options.DataToLoad = 'IMMFrontal1Gray-G-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

%% Training on Pie and testing on both
options.TrainingDataLocation = {'Data\multiPie\experiment\fitting\a1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'PieFrontal1Gray-G-Pyr3_200';

options.LoadTrainingData = true;
options.DataToLoad = 'PieFrontal1Gray-G-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

%% Training on Pie and IMM and testing on both
options.TrainingDataLocation = {'Data\imm\frontal\1', 'Data\multiPie\experiment\fitting\a1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMPieFrontal1Gray-G-Pyr3_200';

options.LoadTrainingData = true;
options.DataToLoad = 'IMMPieFrontal1Gray-G-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

%% Colour spec
options.ColourScheme = 'I1I2I3';
options.BandNormalisation = 'Channel';

% Using the inferred parameters from previous pyramid steps, or just scale
% rotation and orientation (compare results numerically).

%% Training on IMM and testing on both
options.TrainingDataLocation = {'Data\imm\frontal\1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMFrontal1I1I2I3-C-Pyr3_200';

options.LoadTrainingData = true;
options.DataToLoad = 'IMMFrontal1I1I2I3-C-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

%% Training on Pie and testing on both
options.TrainingDataLocation = {'Data\multiPie\experiment\fitting\a1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'PieFrontal1I1I2I3-C-Pyr3_200';

options.LoadTrainingData = true;
options.DataToLoad = 'PieFrontal1I1I2I3-C-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

%% Training on Pie and IMM and testing on both
options.TrainingDataLocation = {'Data\imm\frontal\1', 'Data\multiPie\experiment\fitting\a1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMPieFrontal1I1I2I3-C-Pyr3_200';

options.LoadTrainingData = true;
options.DataToLoad = 'IMMPieFrontal1I1I2I3-C-Pyr3_200.mat';

[TrainingData, Model] = AAMTraining(options);

%EvaluateModel(TrainingData, Model, 'Data\imm\frontal\2', false, true, false, options);
EvaluateModel(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, false, options);

