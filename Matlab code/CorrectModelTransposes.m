function [ Model ] = CorrectModelTransposes( Model )
%CorrectModelTransposes Corrects the model to mimimise the number of
%transposes need in the C++ code, for efficiency
%   Detailed explanation goes here

    Model.App.modes_inv = Model.App.modes';
    
    Model.AgeEst.coeffs = Model.AgeEst.coeffs';
    Model.AgeEst.Transform.offset = Model.AgeEst.Transform.offset';
    Model.AgeEst.Transform.scale = Model.AgeEst.Transform.scale';

end

