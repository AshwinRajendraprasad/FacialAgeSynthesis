function [ Cn, Tn, Recording, numIters, errorAppearance ] = ModelFittingCootes( InputImages, T, C, Model, options )
%MODELFITTINGCOOTES Use the fixed Jacobian approach to AAM fitting starting
%with the given parameters T and C, and using Model,
% Return new parameters, the recording of the fitting if necessary, the
% number of iterations it took to get there, and the current error estimate

    % Go through different resolutions, starting with the coarsest one

    k = [1, 1.5, 0.5, 0.25, 0.125, 0.0625];

    % only if there is more that 5% change in the error continue iterating
    convergenceE = 0.05;

    % Rescale the current estimate of T
    T = T / 2 ^ (numel(Model) - 1);         

    if(options.RecordFitting)   
       Recording = options.Recording; 
    else
       Recording = 0; 
    end
    
    numIters = 0;
    
    for l = numel(Model):-1:1
        
        invWs = inv(Model(l).CombinedModel.Ws);

        if(options.CootesBestFit)
            ErrorTemp = zeros(numel(k), 1);
            Ttemp = zeros(numel(k), numel(T)); 
            Ctemp = zeros(numel(k), numel(C));    
            ErrorVectorTemp = zeros(numel(k), numel(Model(l).AppearanceModel.MeanTexture));
        end
        
        cMin = - 3 * sqrt(Model(l).CombinedModel.Variances);
        cMax = 3 * sqrt(Model(l).CombinedModel.Variances);
    
        % Have a maximum number of iterations
        while numIters < options.MaxIterationsCootes

            numIters = numIters + 1;
            
            % Calculate the current error vector
            B = Model(l).CombinedModel.PrincipalComponents * C;
            Bs = invWs * B(1:numel(Model(l).ShapeModel.Variances));
            Bg = B(numel(Model(l).ShapeModel.Variances) + 1: end);    

            % displaying the convergence
            if(options.verbose)
                figure;
                subplot(2,1,1);
                Shape = Params2Shape(Model(l).ShapeModel.PrincipalComponents, Bs, Model(l).ShapeModel.MeanShape, T);                
                imshow(DrawObject(Shape(:,1)',Shape(:,2)', Model(l).ShapeModel.paths, Model(l).ShapeModel.types, InputImages(l).I));
                subplot(2,1,2);
                imshow(DrawTextureOnTop(InputImages(l).I, Model(l), Bs, Bg, T, Model(l).AppearanceModel.Transform.TranslateGlobal, Model(l).AppearanceModel.Transform.ScaleGlobal));
            end

            if(options.RecordFitting)            
                Image = DrawTextureOnTop(InputImages(l).I, Model(l), Bs, Bg, T, Model(l).AppearanceModel.Transform.TranslateGlobal, Model(l).AppearanceModel.Transform.ScaleGlobal);                
                Image = imresize(Image, 2 ^ (l - 1));
                % to avoid the problem of imresize returning values outside
                % [0;1] due to anti-aliasing filtering
                Image(Image>1) = 1;
                Image(Image<0) = 0;
                
                image(Image,'parent',options.hax);
                
                Recording = addframe(Recording, options.hf);
            end
            
            Shape = Params2Shape(Model(l).ShapeModel.PrincipalComponents, Bs, Model(l).ShapeModel.MeanShape, T);
            
            [textureVector, ~] = CreateVectorFromAppearance(InputImages(l).I, Shape, Model(l).AppearanceModel.ControlPoints, Model(l).AppearanceModel.TextureDimensions, Model(l).AppearanceModel.Triangulation, options);
                
            % Texture normalisation can either be local (per channel) or global
            % For global there will only be 1 mean texture, whilst for
            % local it will be a mean texture per band
            textureVector = NormaliseTexture(textureVector, Model(l).AppearanceModel.MeanTexture, Model(l).AppearanceModel.NumBands...
                            ,Model(l).AppearanceModel.BandSize, Model(l).ColorNorm, strcmpi(Model(l).AppearanceModel.BandNormalisation, 'Global'));

                            
            SynthTexture = Params2Texture(Model(l).AppearanceModel.PrincipalComponents, Bg, Model(l).AppearanceModel.MeanTexture, 0, 1);

            ErrorVector = SynthTexture - textureVector;

            Error = sum(ErrorVector.^2);

            Tdisp = Model(l).T * ErrorVector;

            % Need to convert the parameters for scale and rotation
            rotOld = atan2(T(2), T(1) + T(2));
            scaleOld = (T(1) + 1) / cos(rotOld);                
           
            newError = false;
            for j = 1: numel(k)

                Tnew = T - k(j) * Tdisp';
                
                rotNew = rotOld - k(j) * Tdisp(1);
                scaleNew = scaleOld * (1 - k(j) * Tdisp(2));
                                
                Tnew(1) = scaleNew * cos(rotNew) - 1;
                Tnew(2) = scaleNew * sin(rotNew);

                % Clamp the values of T which are outside the +- 3 std
                Tnew = max(Tnew, Model(l).ShapeModel.TMin');
                Tnew = min(Tnew, Model(l).ShapeModel.TMax');
                
                Shape = Params2Shape(Model(l).ShapeModel.PrincipalComponents, Bs, Model(l).ShapeModel.MeanShape, Tnew);    
                [textureVector, ~] = CreateVectorFromAppearance(InputImages(l).I, Shape, Model(l).AppearanceModel.ControlPoints, Model(l).AppearanceModel.TextureDimensions, Model(l).AppearanceModel.Triangulation,options);  
                               
                % TextureToNormalise, MeanTexture, NumBands, BandSize, Normalise, Global)
                textureVector = NormaliseTexture(textureVector, Model(l).AppearanceModel.MeanTexture, Model(l).AppearanceModel.NumBands...
                                ,Model(l).AppearanceModel.BandSize, Model(l).ColorNorm, strcmpi(Model(l).AppearanceModel.BandNormalisation, 'Global'));
                
                SynthTexture = Params2Texture(Model(l).AppearanceModel.PrincipalComponents, Bg, Model(l).AppearanceModel.MeanTexture, 0, 1);

                ErrorVectorN = SynthTexture - textureVector;
                
                NewError = sum(ErrorVectorN.^2);

                if(options.CootesBestFit)
                    Ttemp(j,:) = Tnew;
                    ErrorTemp(j) = NewError;
                    ErrorVectorTemp(j,:) = ErrorVectorN;
                end                
                
                if(~options.CootesBestFit && (Error - NewError)/Error > convergenceE)
                    T = Tnew;
                    newError = true;
                    break;
                end

            end    

            if(options.CootesBestFit)
                % Get the set of parameters with the smallest error
                [err,Ind] = min(ErrorTemp);
                
                if((Error - err)/Error > convergenceE)
                    T = Ttemp(Ind,:);
                    NewError = err;
                    newError = true;
                    ErrorVectorN = ErrorVectorTemp(Ind,:)';
                end                
                
            end
            
            if(newError)
                ErrorAfterSimilarity = NewError;
                ErrorVector = ErrorVectorN;
            else
                ErrorAfterSimilarity = Error;
            end
            
            % After translation step we use a parameters one
            Cdisp = Model(l).R * ErrorVector;

            for j = 1: numel(k)

                Cnew = C - k(j) * Cdisp;

                % Clamp the values of C which are outside the +- 3 std
                Cnew = max(Cnew, cMin);
                Cnew = min(Cnew, cMax);

                BNew = Model(l).CombinedModel.PrincipalComponents * Cnew;
                BsNew = invWs * BNew(1:numel(Model(l).ShapeModel.Variances));            
                BgNew = BNew(numel(Model(l).ShapeModel.Variances) + 1: end);    

                Shape = Params2Shape(Model(l).ShapeModel.PrincipalComponents, BsNew, Model(l).ShapeModel.MeanShape, T);
                [textureVector, ~] = CreateVectorFromAppearance(InputImages(l).I, Shape, Model(l).AppearanceModel.ControlPoints, Model(l).AppearanceModel.TextureDimensions, Model(l).AppearanceModel.Triangulation,options);  

                textureVector = NormaliseTexture(textureVector, Model(l).AppearanceModel.MeanTexture, Model(l).AppearanceModel.NumBands...
                                ,Model(l).AppearanceModel.BandSize, Model(l).ColorNorm, strcmpi(Model(l).AppearanceModel.BandNormalisation, 'Global'));
                
                SynthTexture = Params2Texture(Model(l).AppearanceModel.PrincipalComponents, BgNew, Model(l).AppearanceModel.MeanTexture, 0, 1);

                ErrorVector = SynthTexture - textureVector;

                NewError = sum(ErrorVector.^2);

                if(options.CootesBestFit)
                    Ctemp(j,:) = Cnew;
                    ErrorTemp(j) = NewError;
                end                                
                
                if( ~options.CootesBestFit && (ErrorAfterSimilarity - NewError)/ErrorAfterSimilarity > convergenceE)
                    C = Cnew;
                    break;
                end

            end        

            if(options.CootesBestFit)
                % Get the set of parameters with the smallest error
                [err,Ind] = min(ErrorTemp);
                
                if((Error - err)/Error > convergenceE)
                    C = Ctemp(Ind,:)';
                    NewError = err;
                end                  
            end            
            
            if((ErrorAfterSimilarity - NewError)/Error < convergenceE)
               break; 
            end

        end
        
        % Transform the parameters for the upper level
        if( l ~= 1)
            
            T = T * 2;
                         
            % Preserve the shape parameters as they don't seem to change
            B = Model(l).CombinedModel.PrincipalComponents * C;
            Bs = invWs * B(1:numel(Model(l).ShapeModel.Variances));
            C = Model(l - 1).CombinedModel.PrincipalComponents' * [Model(l - 1).CombinedModel.Ws * Bs; zeros(numel(Model(l-1).AppearanceModel.Variances), 1)];            
            
        end
        
        
        
    end
    
    % Show the final result
    if(options.verbose)

        BNew = Model(1).CombinedModel.PrincipalComponents * C;
        BsNew = inv(Model(1).CombinedModel.Ws) * BNew(1:numel(Model(1).ShapeModel.Variances));
        BgNew = BNew(numel(Model(1).ShapeModel.Variances) + 1: end);            

        figure;
        subplot(2,1,1);            
        Shape = Params2Shape(Model(1).ShapeModel.PrincipalComponents, BsNew, Model(1).ShapeModel.MeanShape, T);        
        imshow(DrawObject(Shape(:,1)',Shape(:,2)', Model(1).ShapeModel.paths, Model(1).ShapeModel.types,InputImages(1).I));    
        subplot(2,1,2);
        imshow(DrawTextureOnTop(InputImages(1).I, Model(1), BsNew, BgNew, T, Model(1).AppearanceModel.Transform.TranslateGlobal, Model(1).AppearanceModel.Transform.ScaleGlobal));
        drawnow;
    end
    
    if(options.RecordFitting)

        BNew = Model(1).CombinedModel.PrincipalComponents * C;
        BsNew = inv(Model(1).CombinedModel.Ws) * BNew(1:numel(Model(1).ShapeModel.Variances));
        BgNew = BNew(numel(Model(1).ShapeModel.Variances) + 1: end);            
        
        Image = DrawTextureOnTop(InputImages(1).I, Model(1), BsNew, BgNew, T, Model(1).AppearanceModel.Transform.TranslateGlobal, Model(1).AppearanceModel.Transform.ScaleGlobal);
        Image = imresize(Image, 2 ^ (l - 1));
        % to avoid the problem of imresize returning values outside
        % [0;1] due to anti-aliasing filtering
        Image(Image>1) = 1;
        Image(Image<0) = 0;

        image(Image,'parent',options.hax);

        Recording = addframe(Recording, options.hf);

    end    
    
    % Draw fitted outline on the last frame, to see error, could also write
    % PtP error with appearance error
    
    Tn = T;
    Cn = C;
    
    errorAppearance = Error / numel(ErrorVector);
    
end

% Normalising the texture, either channel wise or global, or not at all
function [texture] = NormaliseTexture(TextureToNormalise, MeanTexture, NumBands, BandSize, Normalise, Global)
    
    if(Normalise)
        
        texture = zeros(numel(TextureToNormalise),1);
        
        % Normalise the whole texture as one or by channel
        if(Global)        
            alpha = dot(MeanTexture, TextureToNormalise) / numel(MeanTexture);

            % normalising the texture vector
            TextureToNormalise = TextureToNormalise - mean(TextureToNormalise);    
            texture = TextureToNormalise ./ alpha;
        else
            for i = 1:NumBands
               
               st = BandSize * (i-1) + 1;
               en = BandSize * i;
               alpha = dot(MeanTexture(:,i), TextureToNormalise(st:en)) / BandSize;
               
               TextureToNormalise(st:en) = TextureToNormalise(st:en) - mean(TextureToNormalise(st:en));    
               texture(st:en) = TextureToNormalise(st:en) ./ alpha;
               
            end
            
        end
    else
        TextureToNormalise = TextureToNormalise - mean(TextureToNormalise); 
        texture = TextureToNormalise ./ std(TextureToNormalise);
    end
end
