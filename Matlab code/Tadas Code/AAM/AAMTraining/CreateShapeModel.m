function [ ShapeModel] = CreateShapeModel( TrainingData, options )
%CREATESHAPEMODEL Creating a statistical model of shape from the training
%data provided

ShapeModel = struct;

% X values from the training data are left - > right
% Y values are top -> bottom

[normalisedX, normalisedY, ShapeModel.MeanShape, ShapeModel.Transform] = ProcrustesAnalysis(TrainingData.xVals, TrainingData.yVals, options);

SxS = ShapeModel.Transform.scale .* cos(ShapeModel.Transform.Rotation)' - 1;
SyS = ShapeModel.Transform.scale .* sin(ShapeModel.Transform.Rotation)';

meanSx = mean(SxS);
stdSx = std(SxS);

meanSy = mean(SyS);
stdSy = std(SyS);

ShapeModel.meanTx = mean(ShapeModel.Transform.offsetX);
ShapeModel.stdTx = std(ShapeModel.Transform.offsetX);

ShapeModel.meanTy = mean(ShapeModel.Transform.offsetY);
ShapeModel.stdTy = std(ShapeModel.Transform.offsetY);

ShapeModel.meanRot = mean(ShapeModel.Transform.Rotation);
ShapeModel.stdRot = std(ShapeModel.Transform.Rotation);

ShapeModel.meanScale = mean(ShapeModel.Transform.scale);
ShapeModel.stdScale = std(ShapeModel.Transform.scale);

ShapeModel.TMin = [meanSx - 3 * stdSx
                   meanSy - 3 * stdSy
                   ShapeModel.meanTx - 3 * ShapeModel.stdTx
                   ShapeModel.meanTy - 3 * ShapeModel.stdTy];

ShapeModel.TMax = [meanSx + 3 * stdSx
                   meanSy + 3 * stdSy
                   ShapeModel.meanTx + 3 * ShapeModel.stdTx
                   ShapeModel.meanTy + 3 * ShapeModel.stdTy];               
               
[ShapeModel.PrincipalComponents, ShapeModel.Variances] = PrincipalComponentAnalysis([normalisedX normalisedY]', options.PreserveShapeVariation);

ShapeModel.NormalisedShapes = [normalisedX'; normalisedY'];

% Visualising the normalised shape of the objects + the variations of the
% shape
if(options.verbose)
    DisplayNormalisedShapes(TrainingData, normalisedX, normalisedY, ShapeModel.MeanShape);
    drawnow('expose')
end

end

