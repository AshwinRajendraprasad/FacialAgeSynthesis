function EvaluateAAMs3DPyramids()

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

options.ColorNorm = true;

options.TextureSize = [200 200];
options.Interpolation = 'BI';

options.PyramidSize = 3;

options.RecordFitting = false;
options.ImportDim = [640 480];

options.OutputFittedImages = true;

%% Colour spec
options.ColourScheme = 'RGB';
options.BandNormalisation = 'Global';

% Using the inferred parameters from previous pyramid steps, or just scale
% rotation and orientation (compare results numerically).

%% Training on IMM and testing on both

options.SaveTraining = false;
options.SaveTrainingLoc = 'pyrTrain3D.mat';

options.LoadTrainingData = false;
options.DataToLoad = 'pyrTrain3D.mat';

options.TrainingDataLocation = {'pyrTrainShp'};

[TrainingData, Model] = ModelTraining3D(options);

EvaluateModel3D(TrainingData, Model, 'pyrTest', false, true, false, options);

end