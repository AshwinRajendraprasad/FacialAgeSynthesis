function [ Cn, Tn, Recording, numIters, errorAppearance ] = GlobalFittingCootes( InputImage, Model, options )
%GLOBALFITTINGCOOTES Will fit the global model without
%   Detailed explanation goes here

    m = 3;
    numC = 10;
    k = 10;
    originalMaxIter = options.MaxIterationsCootes;
    
    % Search ranges x - every 40 px, y - every 30 px (this is at 640 by 480
    % resolution), at scales 90%, 110%. 
    
    xStep = 40;
    yStep = 30;
    
    options.MaxIterationsCootes = m;
    
    [M,N,~] = size(InputImage);
    
    % Need the width of the face for search parameters, as not necessary to
    % start searching at the edges
    
    rot = Model(1).ShapeModel.meanRot;
    scale = Model(1).ShapeModel.meanScale;

    sx = scale * cos(rot) - 1;
    sy = scale * sin(rot);    
    
    % create a mean face
    Shape = Params2Shape(Model(1).ShapeModel.PrincipalComponents, zeros(numel(Model(1).ShapeModel.Variances),1), Model(1).ShapeModel.MeanShape, [sx sy 0 0]);
    
    % find extreme x's and y's to get the rough size of the face when it is
    % in the reference frame of the original images used for training
    faceWidth = abs(min(Shape(1,:)) - max(Shape(1,:)));
    faceHeight = abs( min(Shape(2,:)) - max(Shape(2,:)));

    % the search values of x's
    xs = faceWidth / 2 : xStep : N - faceWidth/2;
    
    % the search values of y's
    ys = faceHeight / 2 : yStep : M - faceHeight/2;
        
    [X,Y,S] = meshgrid(xs, ys, scale);
        
    ModelInit = Model;
    
    C = zeros(numel(ModelInit(end).CombinedModel.Variances),1);
    
    % initialise to fake high values
    errors = 100 * ones(numC, 1);
    Ts = zeros(numC, 4);    
    
    InputImages(1).I = impyramid (InputImage, 'reduce');
    for i = 2:numel(Model)
       InputImages(i).I = impyramid (InputImages(i-1).I, 'reduce');
    end        
    
    for i=1:numel(X)
        
        sx = S(i) * cos(rot) - 1;
        sy = S(i) * sin(rot);    
        T = [sx, sy, X(i), Y(i)];
        
        T = T / 2;
        
        [ ~, Tn, ~, ~, errorAppearance ] = ModelFittingCootes( InputImages, T, C, ModelInit, options );
        
        if( max(errors) > errorAppearance)
            [~, ind] = max(errors);
            errors(ind) = errorAppearance;
            Ts(ind,:) = Tn;
        end
        fprintf('%d,',i);
    end
    fprintf('\n');
    
    options.MaxIterationsCootes = k;
    
    Ts = Ts .* 2;
    
    % Use normal size for next steps
    InputImages(1).I = InputImage;
    for i = 2:numel(Model)
       InputImages(i).I = impyramid (InputImages(i-1).I, 'reduce');
    end    
    
    for i=1:numC
       [ ~, Ts(i,:), ~, ~, errors(i) ] = ModelFittingCootes( InputImages, Ts(i,:), C, Model, options );
    end    
    
    % get the one with best error    
    [~,ind] = min(errors);
    options.MaxIterationsCootes = originalMaxIter;
    
    [ Cn, Tn, Recording, numIters, errorAppearance ] = ModelFittingCootes( InputImages, Ts(ind,:), C, Model, options );
    
end

