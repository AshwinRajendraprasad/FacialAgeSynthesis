%% Initialisation
clear ; close all; clc

%% Setup the data

% load('cpusmall.mat');
% load('pyrim.mat');
load('triazines.mat');

% convert from sparse to full matrix
instance_matrix = full(instance_matrix);

numSamples = numel(label_vector);
numFeats = size(instance_matrix,2);

% Make sure the input lies within [0, 1] (to make sure that each feature
% has a chance to have the same effect)
normalisation = diag(1./(max(instance_matrix,[],1)-min(instance_matrix,[],1))');
normalisation(normalisation==inf) = 1;
instance_matrix = (instance_matrix -repmat(min(instance_matrix,[],1),numSamples,1))*normalisation;

input_layer_size  = numFeats;  % 20x20 Input Images of Digits
hidden_layer_size = 10;   % 1 hidden unit (can add more)
num_labels = 1;           

y = label_vector;

offset = - min(label_vector);
y = (label_vector +offset);
scale = 1/max(y);
y = y * scale;
X = instance_matrix;

% take half of the data as test data
indices = randperm(numSamples);
train_inds = indices(1:end/2);
test_inds = indices(end/2+1:end);

%% Initialise params
labelsTrain = y(train_inds);
instancesTrain = X(train_inds,:);

initial_Theta1 = randInitializeWeights(input_layer_size, hidden_layer_size);
initial_Theta2 = randInitializeWeights(hidden_layer_size, num_labels);

% Unroll parameters
initial_nn_params = [initial_Theta1(:) ; initial_Theta2(:)];

%  After you have completed the assignment, change the MaxIter to a larger
%  value to see how more training helps.
options = optimset('MaxIter', 50, 'Display', 'off');

%  You should also try different values of lambda
lambda = 0;

% Create "short hand" for the cost function to be minimized
costFunction = @(p) nnCostFunction(p, ...
                                   input_layer_size, ...
                                   hidden_layer_size, ...
                                   num_labels, X, y, lambda);

% Now, costFunction is a function that takes in only one argument (the
% neural network parameters)
[nn_params, cost] = fmincg(costFunction, initial_nn_params, options);

% Obtain Theta1 and Theta2 back from nn_params
Theta1 = reshape(nn_params(1:hidden_layer_size * (input_layer_size + 1)), ...
                 hidden_layer_size, (input_layer_size + 1));

Theta2 = reshape(nn_params((1 + (hidden_layer_size * (input_layer_size + 1))):end), ...
                 num_labels, (hidden_layer_size + 1));

labelsTest = label_vector(test_inds);
instancesTest = instance_matrix(test_inds,:);

prediction = predict(Theta1, Theta2, instancesTest);

prediction = prediction / scale - offset;

errs = mean((prediction - labelsTest).^2);
corrs = corr(labelsTest, prediction)^2;

fprintf('Corr %.3f, RMS %.3f\n', corrs, errs);
