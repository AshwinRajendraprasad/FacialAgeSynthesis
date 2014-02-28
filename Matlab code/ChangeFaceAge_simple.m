function [ aged_b ] = ChangeFaceAge_simple( Model, novel_texture, target_age, gender, b )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    if gender=='m'
        simple = Model.AgeSynth.male;
    else
        simple = Model.AgeSynth.female;
    end
    
    % apply the simple ageing part
    if nargin<5
        b = FindModelParameters(Model.App, novel_texture);
    end
    del_age = target_age - PredictAge(Model.AgeEst, b);
    if del_age<0
        aged_b = b + (-del_age*simple.younger.fac)*simple.younger.del_b;
    else
        aged_b = b + (del_age*simple.older.fac*2)*simple.older.del_b;
    end
    
    % Clamp the parameters so within 3 times std dev.
    stddev = sqrt(Model.App.variances);
    aged_b(aged_b > 3*stddev) = 3*stddev(aged_b > 3*stddev);
    aged_b(aged_b < -3*stddev) = -3*stddev(aged_b < -3*stddev);
end

