function LoadDependencies

%Add functions path to matlab search path
functionname='LoadDependencies.m'; functiondir=which(functionname);
functiondir=functiondir(1:end-length(functionname));
addpath([functiondir '/AAMFitting'])
addpath([functiondir '/AAMFitting/InverseComp'])
addpath([functiondir '/AAMTraining'])
addpath([functiondir '/AAMTraining/InverseComp'])
addpath([functiondir '/Functions'])
addpath([functiondir '/TestingSuite'])
addpath([functiondir '/labeling'])
addpath([functiondir '/AutoLandmarking'])
addpath([functiondir '/aam3d'])

end

