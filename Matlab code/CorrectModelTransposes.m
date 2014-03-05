function [ New_Model ] = CorrectModelTransposes( Model )
%CorrectModelTransposes Corrects the model to mimimise the number of
%transposes need in the C++ code, for efficiency
%   Detailed explanation goes here

    New_Model = Model;
    New_Model.App.modes_inv = Model.App.modes';
    
    New_Model.AgeEst.coeffs = Model.AgeEst.coeffs';
    New_Model.AgeEst.Transform.offset = Model.AgeEst.Transform.offset';
    New_Model.AgeEst.Transform.scale = Model.AgeEst.Transform.scale';

end

