function EvaluateAAMs3DBhamton()

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

options.TextureSize = [100 100];
options.Interpolation = 'BI';

options.PyramidSize = 3;

options.RecordFitting = false;
options.ImportDim = [320 240];

options.OutputFittedImages = true;

%% Colour spec
%options.ColourScheme = 'RGB';
%options.BandNormalisation = 'Global';
options.ColourScheme = 'RGB';
options.BandNormalisation = 'Global';

%% Training on IMM and testing on both

options.SaveTraining = false;
options.SaveTrainingLoc = 'bhanTrain3D.mat';

options.LoadTrainingData = false;
options.DataToLoad = 'bhamTrain3D.mat';

options.TrainingDataLocation = {'E:\Databases\3dImg\all\train'};
%%
[TrainingData, Model] = ModelTraining3D(options);

EvaluateModel3D(TrainingData, Model, 'bham', false, true, false, options);

end