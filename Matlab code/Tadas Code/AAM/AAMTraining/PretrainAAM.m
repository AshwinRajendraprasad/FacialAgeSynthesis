function [ R, T ] = PretrainAAM( TrainingData, Model, options )
%PRETRAINAAM Calculate the inverse of dr/dp from training data
%   Detailed explanation goes here
    
    % Because we're averaging over a number of training examples can
    % accumulate these instead of storing all of them in memory
    drdp = zeros([numel(Model.CombinedModel.Variances), 4, numel(Model.AppearanceModel.MeanTexture)]);
    dtdp = zeros([4, 6, numel(Model.AppearanceModel.MeanTexture)]);    
    
    dtx = [-6, -3, -1, 1, 3, 6];
    dty = [-6, -3, -1, 1, 3, 6];
    dtr = 0.0174532925 * [-5, -3, -1, 1, 3, 5];
    dts = [ .95, .97, .99, 1.01, 1.03, 1.05 ];
        
    BsAll = (Model.ShapeModel.PrincipalComponents') * Model.ShapeModel.NormalisedShapes;
    BgAll = Model.AppearanceModel.PrincipalComponents' * Model.AppearanceModel.TextureVectors;    
    
    CAll = Model.CombinedModel.PrincipalComponents' * [Model.CombinedModel.Ws * BsAll; BgAll];
    
    parameterVariations = [-.50, -.25, .25, .50];
                
    transParamKernel = exp ((-dtx(:).^2) / (2^2))./dtx(:);
    rotParamKernel = exp ((-dtr(:).^2) / (2*std(dtr)^2))./dtr(:);
    scaleParamKernel = exp ((-(dts(:)-1).^2) / (2*std((dts(:)-1))^2))./(dts(:)-1);
    
    % Scale and offset first (no smoothing for now)
    for i = 1:TrainingData.NumExamples
        
        % first extract the parameters of the model from the training data
        % need the translation parameters as well
        Bs = BsAll(:,i);
                       
        rot = Model.ShapeModel.Transform.Rotation(i);
        scale = Model.ShapeModel.Transform.scale(i);
        
        sx = scale * cos(rot) - 1;
        sy = scale * sin(rot);
        tx = Model.ShapeModel.Transform.offsetX(i);
        ty = Model.ShapeModel.Transform.offsetY(i);
        
        % Extract the texture parameters (plus compensate for shift and
        % scaling)
        
        Bg = BgAll(:, i);        
        
        % Translation in x
        for k=1:numel(dtx)
            txN = tx + dtx(k);                     
            residual = FindResidual(TrainingData.Images(i).I, Model, Bs, Bg, [sx, sy, txN, ty], options);
            dtdp(3, k, :) = squeeze(dtdp(3, k, :)) + transParamKernel(k) * residual;                                           
        end                    
        % Translation in y
        for k=1:numel(dty)
            tyN = ty + dty(k);                       
            residual = FindResidual(TrainingData.Images(i).I, Model, Bs, Bg, [sx, sy, tx, tyN], options);
            dtdp(4, k, :) = squeeze(dtdp(4, k, :)) + transParamKernel(k) * residual;      
        end
        % Rotation
        for k=1:numel(dtr)

            rotN = rot + dtr(k);                        

            sxN = scale * cos(rotN) - 1;
            syN = scale * sin(rotN);

            residual = FindResidual(TrainingData.Images(i).I, Model, Bs, Bg, [sxN, syN, tx, ty], options);                       
            dtdp(1, k, :) = squeeze(dtdp(1, k, :)) + rotParamKernel(k) * residual;                              

        end
        % Scaling
        for k=1:numel(dts)

            sN = scale * dts(k);

            sxN = sN * cos(rot) - 1;
            syN = sN * sin(rot);

            residual = FindResidual(TrainingData.Images(i).I, Model, Bs, Bg, [sxN, syN, tx, ty], options);                        
            dtdp(2, k, :) = squeeze(dtdp(2, k, :)) + scaleParamKernel(k) * residual;                              

        end
        fprintf('Example %d done T\n', i);
    end
    
    WsInv = inv(Model.CombinedModel.Ws);
    
    for i = 1:1:TrainingData.NumExamples

        % first extract the parameters of the model from the training data
        % need the translation parameters as well
        Bs = BsAll(:,i);
                       
        rot = Model.ShapeModel.Transform.Rotation(i);
        scale = Model.ShapeModel.Transform.scale(i);
        
        sx = scale * cos(rot) - 1;
        sy = scale * sin(rot);
        tx = Model.ShapeModel.Transform.offsetX(i);
        ty = Model.ShapeModel.Transform.offsetY(i);
        
        % Extract the texture parameters (plus compensate for shift and
        % scaling)
        
        Bg = BgAll(:, i);

        % extract c
        
        C = Model.CombinedModel.PrincipalComponents' * [Model.CombinedModel.Ws * Bs; Bg];
        
        % For debug purposes (to see if the params are extracted properly)
        if(options.verbose)
            DrawTextureOnTop(TrainingData.Images(i).I, Model, Bs, Bg, [sx,sy,tx,ty]);
        end
        
        for j = 1:numel(Model.CombinedModel.Variances)
            
            % The parameters of the model (C)
           
            % First get the variance of the current parameter
            var = std(CAll(j,:));

            paramsChange = var * parameterVariations;
            
            smoothKernel = exp ((-paramsChange(:).^2) / (2*std(paramsChange)^2))./paramsChange(:);

            for k = 1:numel(parameterVariations)

                CNew = C;
                CNew(j) = CNew(j) + paramsChange(k);

                B = Model.CombinedModel.PrincipalComponents * CNew;
                BsN = WsInv * B(1:numel(Model.ShapeModel.Variances));
                BgN = B(numel(Model.ShapeModel.Variances) + 1: end);

                residual = FindResidual(TrainingData.Images(i).I, Model, BsN, BgN, [sx, sy, tx, ty], options);
                drdp(j, k, :) = squeeze(drdp(j, k, :)) + smoothKernel(k) * residual;

            end
            
        end
        
        fprintf('Example %d done C\n', i);
        drawnow;
    end
    
    % average over all training images first
    
    % might need to clear up memory use to perform this    
    
    dr = drdp ./ TrainingData.NumExamples;
    dr = squeeze(mean(dr, 2));
    
    dt = dtdp ./ TrainingData.NumExamples;
    
    dt = squeeze(mean(dt, 2));
    
    % now average over the different deviations
    
    % Find the pseudo inverse giving us R
    T = inv(dt*dt')*dt;
    
    R = inv(dr*dr')*dr;
    
%     % visualise T
%     for i = 1:4
%         Tvis = Vec2TexCol(T(i,:), Model.AppearanceModel.TextureMap, Model.AppearanceModel.TextureDimensions,Model.AppearanceModel.NumBands, Model.AppearanceModel.BandSize);
%         Tvis = Tvis - min(min(min(Tvis)));
%         Tvis = Tvis ./ max(max(max(Tvis)));
%         figure;
%         imshow(Tvis);
%     end
%     
%     % visualise R        
%     for i = 1:4
%         Rvis = Vec2TexCol(R(i,:), Model.AppearanceModel.TextureMap, Model.AppearanceModel.TextureDimensions,Model.AppearanceModel.NumBands, Model.AppearanceModel.BandSize);
%         Rvis = Rvis - min(min(min(Rvis)));
%         Rvis = Rvis ./ max(max(max(Rvis)));
%         figure;
%         imshow(Rvis);
%     end    
end

function [residual]=FindResidual(CurrentImage, Model, Bs, Bg, T, options)
    Shape = Params2Shape(Model.ShapeModel.PrincipalComponents, Bs, Model.ShapeModel.MeanShape, T);
    
    [textureVector, ~] = CreateVectorFromAppearance(CurrentImage, Shape, Model.AppearanceModel.ControlPoints, options.TextureSize,Model.AppearanceModel.Triangulation,options);
        
    if(Model.ColorNorm)
        
        % Normalise the whole texture as one or by channel
        if(strcmpi(Model.AppearanceModel.BandNormalisation,'Global'))
            alpha = dot(Model.AppearanceModel.MeanTexture, textureVector) / numel(textureVector);

            % normalising the texture vector
            textureVector = textureVector - mean(textureVector);    
            textureVector = textureVector ./ alpha;
        else
            
            for i = 1:Model.AppearanceModel.NumBands
               
               st = Model.AppearanceModel.BandSize * (i-1) + 1;
               en = Model.AppearanceModel.BandSize * i;
               alpha = dot(Model.AppearanceModel.MeanTexture(:,i), textureVector(st:en)) / Model.AppearanceModel.BandSize;
               
               textureVector(st:en) = textureVector(st:en) - mean(textureVector(st:en));    
               textureVector(st:en) = textureVector(st:en) ./ alpha;
               
            end
            
        end
    else
        textureVector = textureVector - mean(textureVector); 
        textureVector = textureVector ./ std(textureVector);
    end
    
    SynthTexture = Params2Texture(Model.AppearanceModel.PrincipalComponents, Bg, Model.AppearanceModel.MeanTexture, 0, 1);
    residual = SynthTexture - textureVector;
    
    if(options.verbose)
        tex = DrawTextureOnTop(CurrentImage, Model, Bs, Bg, T, Model.AppearanceModel.Transform.TranslateGlobal, Model.AppearanceModel.Transform.ScaleGlobal);
        imshow(Model.ConvertFrom(tex));
    end
    
end
