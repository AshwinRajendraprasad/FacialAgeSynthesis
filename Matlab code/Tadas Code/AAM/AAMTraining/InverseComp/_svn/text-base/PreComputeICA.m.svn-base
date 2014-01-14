function [ SimilarityTransform, SimilarityParams, Hessian, SD, Model ] = PreComputeICA(Model)
%PRECOMPUTE Pre-compute the necessary components for the ICA
%   Precompute necessary components for the implementation of Matthews and
%   Baker AAM model

    AppearanceModel = Model(1).AppearanceModel;
    ShapeModel = Model(1).ShapeModel;
    
    % Gradient of the template
    
    % Get the template from the base texture mesh

    % Convert from an array to a 2D representation
    %Texture = Vec2TexCol(Model.Template, AppearanceModel.TextureMap, AppearanceModel.TextureDimensions, AppearanceModel.NumBands, AppearanceModel.BandSize);
    Texture = Vec2TexCol(AppearanceModel.MeanTexture, AppearanceModel.TextureMap, AppearanceModel.TextureDimensions, AppearanceModel.NumBands, AppearanceModel.BandSize);
    
    % Dealing with multi-band
    GradX = zeros([AppearanceModel.NumBands AppearanceModel.TextureDimensions]);
    GradY = zeros([AppearanceModel.NumBands AppearanceModel.TextureDimensions]);
    
    for b = 1:AppearanceModel.NumBands
        [GradX(b,:,:), GradY(b,:,:)] = gradient(Texture(:,:,b));
    end
    
    % Calculating dW/dx and dW/dy
    
    numVerts = size(AppearanceModel.ControlPoints, 1);
    
    dWdX = zeros([ numVerts AppearanceModel.TextureDimensions]);
    dWdY = zeros([ numVerts AppearanceModel.TextureDimensions]);
    
    % Some speed ups
    
    M = AppearanceModel.TextureDimensions(1);
    N = AppearanceModel.TextureDimensions(2);
    
    y = 1:N;
    
    for i=1:M
       
        qpoints((i-1)*N+1:i*N,1) = i*ones(N,1);
        qpoints((i-1)*N+1:i*N,2) = y(:);
        
    end    
    
    [tetids, ~] = pointLocation(AppearanceModel.Triangulation, qpoints);
    
    [xs, ys] = meshgrid(1:M, 1:N);            
    
    % For every triangle    
    for i = 1:size(AppearanceModel.Triangulation.Triangulation, 1)
       
        tri = AppearanceModel.Triangulation.Triangulation(i,:);
        % For every vertex in the triangle        
        for j = 1: 3
           
            v1x = AppearanceModel.ControlPoints(tri(j),1);
            v1y = AppearanceModel.ControlPoints(tri(j),2);
            
            v2x = AppearanceModel.ControlPoints(tri(mod(j,3) + 1),1);
            v2y = AppearanceModel.ControlPoints(tri(mod(j,3) + 1),2);
            
            v3x = AppearanceModel.ControlPoints(tri(mod(j-2,3) + 1),1);
            v3y = AppearanceModel.ControlPoints(tri(mod(j-2,3) + 1),2);
        
            [dWdXinc, dWdYinc] = GetDwdXi(xs, ys, tetids==i, v1x, v1y, v2x, v2y, v3x, v3y);
            
            dWdX(tri(j),:) = dWdX(tri(j),:) + dWdXinc';
            dWdY(tri(j),:) = dWdY(tri(j),:) + dWdYinc';
            
        end
        
    end

    % dWdX, and dWdY contain the dW/dx values per vertex (eq 30, AAM revis)
    
    % Now calculate dX/dP            
    dxdp = ShapeModel.PrincipalComponents(1:end/2,:);
    dydp = ShapeModel.PrincipalComponents(end/2+1:end,:);        
    
    % now calculate dWdP (eq 29 AAM revis)
    numShapeVar = size(ShapeModel.PrincipalComponents,2);
    numAppVar = size(AppearanceModel.PrincipalComponents,2);
    
    dWdPx = zeros([numShapeVar AppearanceModel.TextureDimensions]);
    dWdPy = zeros([numShapeVar AppearanceModel.TextureDimensions]);
    
    % Sum over all vertices
    for i = 1:numVerts
        % Do each param separately (there should be away to optimise this
        % loop)
        for p = 1:numShapeVar
            dWdPx(p,:,:) = dWdPx(p,:,:) + dWdX(i,:,:) * dxdp(i,p);
            dWdPy(p,:,:) = dWdPy(p,:,:) + dWdY(i,:,:) * dydp(i,p);
        end
    end
    
    % Now calculate dNdq (similarity transform Jacobian)
    
    s1 = [ ShapeModel.MeanShape(:,1)' ShapeModel.MeanShape(:,2)'];
    s2 = [ - ShapeModel.MeanShape(:,2)' ShapeModel.MeanShape(:,1)'];
    s3 = [ ones(size(ShapeModel.MeanShape,1),1)' zeros(size(ShapeModel.MeanShape,1), 1)' ];
    s4 = [ zeros(size(ShapeModel.MeanShape,1),1)' ones(size(ShapeModel.MeanShape,1), 1)' ];
        
    % Now orthonormalise and create an orthonormalisation multiplier to
    % convert from similarity transforms sx,sy,tx,ty to q1,q2,q3,q4
    s1Mult = norm(s1);
    s1 = s1 / s1Mult;
    s2Mult = norm(s2);
    s2 = s2 / s2Mult;
    s3Mult = norm(s3);
    s3 = s3 / s3Mult;
    s4Mult = norm(s4);
    s4 = s4 / s4Mult;    
   
    SimilarityTransform = diag([s1Mult s2Mult, s3Mult, s4Mult]);
    SimilarityParams = [s1', s2', s3', s4'];    
    
    % Now calculate dN/dq            
    dxdq = SimilarityParams(1:end/2,:);
    dydq = SimilarityParams(end/2+1:end,:);        
    
    % now calculate dNdq (eq 29 AAM revis)
    
    dNdqx = zeros([4 AppearanceModel.TextureDimensions]);
    dNdqy = zeros([4 AppearanceModel.TextureDimensions]);
    
    % Sum over all vertices
    for i = 1:numVerts
        % Do each param separately
        for p = 1:4
            dNdqx(p,:,:) = dNdqx(p,:,:) + dWdX(i,:,:) * dxdq(i,p);
            dNdqy(p,:,:) = dNdqy(p,:,:) + dWdY(i,:,:) * dydq(i,p);
        end
    end
    
    % Calculate the steepest descent (step 5 in ICA algorithm)    
    steepestDescent = zeros([numShapeVar + 4 AppearanceModel.NumBands AppearanceModel.TextureDimensions]);
    steepestDescentVectors = zeros([numShapeVar + 4 AppearanceModel.BandSize * AppearanceModel.NumBands]);
    BandSize = AppearanceModel.BandSize;
    
    % for each similarity parameter
    for q = 1:4
        % For each band
        for b =1:AppearanceModel.NumBands
            % Over both x and y directions
            steepestDescent(q, b,:,:) = dNdqx(q,:,:) .* GradX(b,:,:) + dNdqy(q,:,:) .* GradY(b,:,:);
            % Convert the steepest descent to vectors for easier handling
            img = squeeze(steepestDescent(q,b,:,:));
            steepestDescentVectors(q, BandSize * (b-1) + 1: BandSize * b) = img(AppearanceModel.TextureMap);
        end
    end
    
    % for each shape parameter
    for p = 1:numShapeVar
    
        % For each band
        for b =1:AppearanceModel.NumBands
            % Over both x and y directions
            steepestDescent(p + 4, b,:,:) = dWdPx(p,:,:) .* GradX(b,:,:) + dWdPy(p,:,:) .* GradY(b,:,:);
            % Convert the steepest descent to vectors for easier handling
            img = squeeze(steepestDescent(p + 4,b,:,:));
            steepestDescentVectors(p + 4, BandSize * (b-1) + 1: BandSize * b) = img(AppearanceModel.TextureMap);
        end
    end               
    
    % Get the modified steepest descent images from eq 41 in Baker and
    % Matthews
    
    % Need to convert the steepest descents to vectors

    ModifiedSDVectors = zeros([numShapeVar + 4 AppearanceModel.BandSize * AppearanceModel.NumBands]);
    
    % For each similarity transform parameter
    for q = 1:4
       
        ModifiedSDVectors(q,:) = steepestDescentVectors(q,:);
        % for each appearance parameter
        
        for l=1:numAppVar
            ModifiedSDVectors(q,:) = ModifiedSDVectors(q,:) - dot(steepestDescentVectors(q,:), AppearanceModel.PrincipalComponents(:,l)) * AppearanceModel.PrincipalComponents(:,l)'; 
        end
        
        % Visualise the modified appearance vector
        %img = Vec2TexCol(ModifiedSDVectors(q,:), AppearanceModel.TextureMap, AppearanceModel.TextureDimensions, AppearanceModel.NumBands, BandSize);
        
    end    
    
    % For each shape parameter
    for p = 1:numShapeVar
       
        ModifiedSDVectors(p + 4,:) = steepestDescentVectors(p + 4,:);
        % for each appearance parameter
        
        for l=1:numAppVar
           ModifiedSDVectors(p + 4,:) = ModifiedSDVectors(p + 4,:) - dot(steepestDescentVectors(p + 4,:), AppearanceModel.PrincipalComponents(:,l)) * AppearanceModel.PrincipalComponents(:,l)';
        end
        
        % Visualise the modified appearance vector
%        img = Vec2TexCol(ModifiedSDVectors(p + 4,:), AppearanceModel.TextureMap, AppearanceModel.TextureDimensions, AppearanceModel.NumBands, BandSize);
        
    end
    
    % Can finally compute the Hessian
    Hessian = ModifiedSDVectors * ModifiedSDVectors';
    SD = ModifiedSDVectors;
    
%       Hessian = steepestDescentVectors * steepestDescentVectors';
%       SD = steepestDescentVectors;

end

function [dWdXi, dWdYi] = GetDwdXi( xs, ys, mask, vxi, vyi, vxj, vyj, vxk, vyk)

    dWdXi = zeros(size(mask));    
    
    denom = (vxj - vxi)*(vyk-vyi) - (vyj - vyi)*(vxk - vxi);
    alphas = ((xs(mask) - vxi)*(vyk - vyi) - (ys(mask) - vyi)*(vxk - vxi)) ./ denom;
    
    betas = ((ys(mask) - vyi)*(vxj - vxi) - (xs(mask) - vxi)*(vyj - vyi)) ./ denom;
    
    dWdXi(mask) = ones(numel(alphas),1) - alphas - betas;
    dWdYi = dWdXi;
    
end
