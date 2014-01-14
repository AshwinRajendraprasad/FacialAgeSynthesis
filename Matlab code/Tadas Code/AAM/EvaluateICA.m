%% Load the model
LoadDependencies;

options = struct;

options.DisplayShapeVariations = false;
options.DisplayAppearanceVariations = false;

options.TangentSpaceTransform = true;

options.PreserveShapeVariation = 0.95;
options.PreserveAppearanceVariation = 0.95;
options.PreserveCombinedVariation = 0.95;

options.ColorNorm = true;

options.verbose = false;

options.TextureSize = [100 100];
options.Interpolation = 'BI';

options.PyramidSize = 3;

options.ColourScheme = 'i1i2i3';
options.BandNormalisation = 'Channel';

options.OutputFittedImages = true;

options.ImportDim = [640 480];

%% Training on IMM and testing on both
options.TrainingDataLocation = {'C:\Users\tb346\Documents\AAM\mindreader\AAM\Data\imm new\all'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMFrontal1L1L2L3-Pyr3_100ICA';

options.LoadTrainingData = true;
options.DataToLoad = 'IMMFrontal1L1L2L3-Pyr3_100ICA.mat';

[TrainingData, Model] = AAMTrainingICA(options);

EvaluateModelICA(TrainingData, Model, 'Data\imm\frontal\2', true, false, options);
%EvaluateModelICA(TrainingData, Model, 'Data\imm\frontal\2', false, true, options);
%EvaluateModelICA(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, options);

%% Training on Pie and testing on both
options.TrainingDataLocation = {'Data\imm\frontal\1', 'Data\multiPie\experiment\fitting\a1'};

options.SaveTraining = false;
options.SaveTrainingLoc = 'IMMPieFrontal1L1L2L3-Pyr3_100ICA';

options.LoadTrainingData = true;
options.DataToLoad = 'IMMPieFrontal1L1L2L3-Pyr3_100ICA.mat';

[TrainingData, Model] = AAMTrainingICA(options);

%EvaluateModelICA(TrainingData, Model, 'Data\imm\frontal\2', true, false, options);
EvaluateModelICA(TrainingData, Model, 'Data\imm\frontal\2', false, true, options);
EvaluateModelICA(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, options);

%% Training on IMM and testing on both
options.TrainingDataLocation = {'Data\imm\frontal\1'};

options.SaveTraining = true;
options.SaveTrainingLoc = 'IMMFrontal1L1L2L3-Pyr3_100ICA';

options.LoadTrainingData = false;
options.DataToLoad = 'IMMFrontal1L1L2L3-Pyr3_100ICA.mat';

[TrainingData, Model] = AAMTrainingICA(options);

%EvaluateModelICA(TrainingData, Model, 'Data\imm\frontal\2', true, false, options);
EvaluateModelICA(TrainingData, Model, 'Data\imm\frontal\2', false, true, options);
EvaluateModelICA(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, options);

%% Training on Pie and testing on both
options.TrainingDataLocation = {'Data\multiPie\experiment\fitting\a1'};

options.SaveTraining = true;
options.SaveTrainingLoc = 'PieFrontal1L1L2L3-Pyr3_100ICA';

options.LoadTrainingData = false;
options.DataToLoad = 'PieFrontal1L1L2L3-Pyr3_100ICA.mat';

[TrainingData, Model] = AAMTrainingICA(options);

%EvaluateModelICA(TrainingData, Model, 'Data\imm\frontal\2', true, false, options);
EvaluateModelICA(TrainingData, Model, 'Data\imm\frontal\2', false, true, options);
EvaluateModelICA(TrainingData, Model, 'Data\multiPie\experiment\fitting\a2', false, true, options);
