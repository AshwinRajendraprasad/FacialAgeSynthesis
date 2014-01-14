function [ SimilarityCoeffs, ShapeCoeffs, AppearanceCoeffs, numIters ] = ICAFitting( Images, Model, InitSimilarityParams, InitShapeParams, maxIter )
%ICAFITTING Fitting the Active Appearance Model to an image using the
%inverse compositional alignment algorithm described by Matthews and Baker
%in Active Appearance Models Revisited (2004)
    
    if nargin == 4
        maxIter = 30;
    end

    % Have a pyramid approach to this
    
    % Rescale the current estimate of T
    InitSimilarityParams = InitSimilarityParams / 2 ^ (numel(Model) - 1);         

    %Get the warped Image from the initial parameters
    q = Model(end).SimilarityTransform * InitSimilarityParams';
    p = InitShapeParams;

    qNew = q;
    pNew = p;
    
    numIters = 0;
       
    for l = numel(Model):-1:1
    
        %% Precomputing step (should probably be taken out to learning step)    
        ShapeModel = Model(l).ShapeModel;
        AppearanceModel = Model(l).AppearanceModel;

        MaxP = 3 * sqrt(ShapeModel.Variances);
        MinP = -MaxP;
        
        MinQ = Model(end).SimilarityTransform * ShapeModel.TMin;
        MaxQ = Model(end).SimilarityTransform * ShapeModel.TMax;
        
        InvHessian = inv(Model(l).Hessian);
        
        BaseShape = [ShapeModel.MeanShape(:,1)' ShapeModel.MeanShape(:,2)']';        
        
        iterLevel = 0;
        
        %% Iterations    
       while numIters < maxIter

            numIters = numIters + 1;
            iterLevel = iterLevel + 1;
                       
            WarpedShape = Params2Shape(ShapeModel.PrincipalComponents, pNew, ShapeModel.MeanShape, qNew'  / Model(l).SimilarityTransform);
            
%             WarpedShape = BaseShape +  ShapeModel.PrincipalComponents * p + Model(l).SimilarityParams * q;
%             WarpedShape = [WarpedShape(1:end/2), WarpedShape(end/2+1:end)];
            
            [I, ~] = PieceWiseQuick(Images(l).I, WarpedShape, AppearanceModel.ControlPoints, AppearanceModel.TextureDimensions, AppearanceModel.Triangulation, 'BI');
            %imshow(I);
            %drawnow expose;

            % Go over all bands and concatenate them
            CurrApp = zeros( AppearanceModel.NumBands * AppearanceModel.BandSize, 1);
            for b = 1:AppearanceModel.NumBands
                B = I(:,:,b);
                st = AppearanceModel.BandSize * (b-1) + 1;
                en = AppearanceModel.BandSize * b;
                CurrApp(st:en) = B(AppearanceModel.TextureMap);
            end            
            
            VecError = GetNormalisedError(CurrApp, Model(l))';
            %VecError = I(AppearanceModel.TextureMap) - Model.Template;
            NewError = sum(VecError.^2);

            % If there was not much improvement terminate?
            if(iterLevel ~= 1 && (OldError - NewError)/OldError < 0.05)
                break; 
            end        

            OldError = NewError;
            q = qNew;
            p = pNew;
            
            %PieceWiseQuick(Image, AppearanceModel.ControlPoints, WarpedShape, AppearanceModel.TextureDimensions, AppearanceModel.Triangulation, 'BI');
            %imshow(DrawTriangulation( Image, AppearanceModel.Triangulation ));

            % Visualise the error
            %img = Vec2TexCol(VecError, AppearanceModel.TextureMap, AppearanceModel.TextureDimensions, AppearanceModel.NumBands, AppearanceModel.BandSize);

            % Step 7 of the ICA
            PUp = InvHessian * (Model(l).SD * VecError);

            % The PUp returned seems to be consistent with WarpedShape = BaseShape +  ShapeModel.PrincipalComponents * p + Model(l).SimilarityParams * q;
            % Thus need to disentangle these
            
            % Step 9 of the ICA

            % Now that the new parameters have been determined we can compose them
            % or add them (worse outcome)
                                    
            [qNew,pNew] = GetNewParameters(BaseShape, AppearanceModel.Triangulation.Triangulation, WarpedShape, ShapeModel.PrincipalComponents, Model(l).SimilarityParams, q, p, PUp, true, Model(l).SimilarityTransform);

            % clamping the parameter values
            pNew = max(pNew, MinP);
            pNew = min(pNew, MaxP);
            
            % clamping q values
            qNew = max(qNew, MinQ);
            qNew = min(qNew, MaxQ);
            
        end
        
        % Transform the parameters for the upper level
        if( l ~= 1)
            
            q = q * 2;
            
            qNew = q;
            % To avoid error drift, due to bad shift due to previous
            % pyramid levels
            pNew = InitShapeParams;
            
        end        
        
    end
    
    ShapeCoeffs = p;
    SimilarityCoeffs = Model(1).SimilarityTransform \ q;
    
    % Its possible that not all pyramid levels have been used up
    if( numIters == maxIter)
        
        WarpedShape = Params2Shape(ShapeModel.PrincipalComponents, p, ShapeModel.MeanShape, q'  / Model(l).SimilarityTransform);
        [I, ~] = PieceWiseQuick(Images(1).I, WarpedShape, AppearanceModel.ControlPoints, AppearanceModel.TextureDimensions, AppearanceModel.Triangulation, 'BI');
        CurrApp = zeros( AppearanceModel.NumBands * AppearanceModel.BandSize, 1);
        for b = 1:AppearanceModel.NumBands
            B = I(:,:,b);
            st = Model(1).AppearanceModel.BandSize * (b-1) + 1;
            en = Model(1).AppearanceModel.BandSize * b;
            CurrApp(st:en) = B(AppearanceModel.TextureMap);
        end             
        VecError = GetNormalisedError(CurrApp, Model(1))';
    end
    
    AppearanceCoeffs = Model(1).AppearanceModel.PrincipalComponents' * VecError;
        
    
end

function ErrorVector = GetNormalisedError( CurrentVector, Model)
    
    if(Model.ColorNorm)
        
        % Normalise the whole texture as one or by channel
        if(strcmpi(Model.AppearanceModel.BandNormalisation,'Global'))
            alpha = dot(Model.AppearanceModel.MeanTexture, CurrentVector) / numel(CurrentVector);

            % normalising the texture vector
            NormalisedVector = CurrentVector - mean(CurrentVector);    
            NormalisedVector = NormalisedVector ./ alpha;
        else
            
            for i = 1:Model.AppearanceModel.NumBands
               
               st = Model.AppearanceModel.BandSize * (i-1) + 1;
               en = Model.AppearanceModel.BandSize * i;
               alpha = dot(Model.AppearanceModel.MeanTexture(:,i), CurrentVector(st:en)) / Model.AppearanceModel.BandSize;
               
               NormalisedVector(st:en) = CurrentVector(st:en) - mean(CurrentVector(st:en));    
               NormalisedVector(st:en) = NormalisedVector(st:en) ./ alpha;
               
            end
            
        end
    else
        NormalisedVector = CurrentVector - mean(CurrentVector); 
        NormalisedVector = NormalisedVector ./ std(NormalisedVector);
    end
        
    ErrorVector = NormalisedVector - reshape(Model.AppearanceModel.MeanTexture, 1, numel(NormalisedVector));

end

function [Similarity, Shape] = GetNewParameters(BaseShape, Triangles, CurrentShape, ShapeComponents, SimilarityComponents, OldSimilarity, OldShape, paramOffset, compose, SimilarityTransform)


    T = inv(SimilarityTransform) * (OldSimilarity - paramOffset(1:4));
    Ta = [1 + T(1), -T(2); T(2), 1 + T(1)];

    ShapeOffset = ShapeComponents * paramOffset(5:end);
    ShapeOffset = [ShapeOffset(1:end/2), ShapeOffset(end/2+1:end)];
    ShapeOffset = ShapeOffset / Ta;    
    ShapeOffset = [ShapeOffset(1:end/2), ShapeOffset(end/2+1:end)]'; 
    ShapeParamOffset = ShapeComponents' * ShapeOffset;
        
    if(compose)
        
        SimilarityOffset = SimilarityComponents * paramOffset(1:4);
        SimilarityOffset = [SimilarityOffset(1:end/2), SimilarityOffset(end/2+1:end)];
        SimilarityOffset = SimilarityOffset / Ta;
        SimilarityOffset = [SimilarityOffset(1:end/2), SimilarityOffset(end/2+1:end)]';
        SimilarityParamOffset = SimilarityComponents' * SimilarityOffset;
                        
        BaseOffset = Params2Shape(ShapeComponents, -ShapeParamOffset, [BaseShape(1:end/2), BaseShape(end/2+1:end)], -SimilarityParamOffset'  / SimilarityTransform);
        %BaseOffset = BaseShape + SimilarityComponents * (-SimilarityParamOffset) + ShapeComponents * (-ShapeParamOffset);
        
        % Now we need to warp the base offset for every triangle and vertex
        % and average over all the triangles the vertex contains
        
        % Every triangle will have a warp matrix
        TriangleWarps = zeros([ size(Triangles,1) 2 3 ]);
        
        numV = numel(BaseShape) / 2;
        
        for t = 1:size(Triangles,1)
            
            xi0 = BaseShape(Triangles(t,1));
            xj0 = BaseShape(Triangles(t,2));
            xk0 = BaseShape(Triangles(t,3));

            xi1 = CurrentShape(Triangles(t,1));
            xj1 = CurrentShape(Triangles(t,2));
            xk1 = CurrentShape(Triangles(t,3));
            
            yi0 = BaseShape(Triangles(t,1) + numV);
            yj0 = BaseShape(Triangles(t,2) + numV);
            yk0 = BaseShape(Triangles(t,3) + numV);

            yi1 = CurrentShape(Triangles(t,1) + numV);
            yj1 = CurrentShape(Triangles(t,2) + numV);
            yk1 = CurrentShape(Triangles(t,3) + numV);
                        
            TriangleWarps(t, :,:) = GetWarpMatrix(xi0, yi0, xj0, yj0, xk0, yk0, xi1, yi1, xj1, yj1, xk1, yk1);
            
        end
        
        % For every vertex find the triangles it's in and then apply the
        % warps to it and average the results
        Vs = zeros(numV, 2);
        for v = 1:numV
            
            % Gives the appropriate triangle warps
            tris = TriangleWarps(sum(Triangles == v,2)==1,:,:);
            
            vert = [BaseOffset(v, 1) BaseOffset(v, 2) 1];
            
            sumV = [0;0];
            
            for t=1:size(tris,1)
                sumV = sumV + squeeze(tris(t,:,:)) * vert';
            end
            
            Vs(v,:) = sumV'/size(tris,1);
            
        end
        
        % Now that we have the vertex locations under the composite warp
        % need to work out the parameters p and q that would produce them
        
        Vs = [Vs(1:end/2) Vs(end/2 + 1 : end)]';
        
        % Using eq 61
        Similarity = SimilarityComponents' * (Vs - BaseShape);
        
        T = inv(SimilarityTransform) * (Similarity);
        Ta = [1 + T(1), -T(2); T(2), 1 + T(1)];

        Vs = [Vs(1:end/2), Vs(end/2+1:end)];
        Vs = Vs / Ta;    
        Vs = [Vs(1:end/2), Vs(end/2+1:end)]'; 
        Shape = ShapeComponents' * (Vs - BaseShape);        
        
        % Can now get the parameters with similarity removed
        %pNew = 
        
    else
        Similarity = OldSimilarity - paramOffset(1:4);  
        %Similarity = OldSimilarity - SimilarityParamOffset;
        %Shape = OldShape - paramOffset(5:end);
        Shape = OldShape - ShapeParamOffset;
    end
end
