function [ Texture, Map ] = PieceWiseQuick( OriginalTexture, ControlPoints, TargetPoints, Size, DT, interpolation )
%PIECEWISEQUICK A simple piece wise affine warp without any precomputed
%values, useful when precomputation is impossible, that is warping to new
%control points all the time, with alpha channel for transparency outside
%   Detailed explanation goes here

    % go through the triangles (min max) and get all of the control warp
    % and trangles map for the pixels in those triangles
    
    M = Size(1);
    N = Size(2);    
    D = size(OriginalTexture, 3);
    
    qpoints = zeros(M*N,2);
    
    y = 1:M;
    
    %Should be replaced with mesh grid
    for i=1:N
       
        qpoints((i-1)*M+1:i*M,1) = i*ones(M,1);
        qpoints((i-1)*M+1:i*M,2) = y(:);
        
    end    

    DT.X = TargetPoints;
    [tetids, bcs] = pointLocation(DT, qpoints);
    
    % Get the non NaN ids
    inds = find(~isnan(tetids));
    
    CPTris = DT.Triangulation(tetids(inds),:);    
    
    CPx = cat(2,ControlPoints(CPTris(:,1), 1), ControlPoints(CPTris(:,2), 1), ControlPoints(CPTris(:,3), 1));
    
    CPy = cat(2,ControlPoints(CPTris(:,1), 2), ControlPoints(CPTris(:,2), 2), ControlPoints(CPTris(:,3), 2));
    
    if (strcmpi(interpolation,'NN'))
    
        xSources = round(dot(bcs(inds,1:3),CPx,2));
        ySources = round(dot(bcs(inds,1:3),CPy,2));    

        xSources(xSources < 1) = 1;
        xSources(xSources > size(OriginalTexture,1)) = size(OriginalTexture,1);

        ySources(ySources < 1) = 1;
        ySources(ySources > size(OriginalTexture,2)) = size(OriginalTexture,2);

        indSources = sub2ind([size(OriginalTexture,1) size(OriginalTexture,2)], ySources, xSources);

        xTargets = qpoints(inds,1);
        yTargets = qpoints(inds,2);
                
        indTargets = sub2ind(Size, yTargets, xTargets);   

        % now that all the mapings are here need to actually map
        TextureR = zeros(M,N);
        TextureG = zeros(M,N);
        TextureB = zeros(M,N);

        OriginalR = OriginalTexture(:,:,1);
        OriginalG = OriginalTexture(:,:,2);
        OriginalB = OriginalTexture(:,:,3);        

        TextureR(indTargets) = OriginalR(indSources);
        TextureG(indTargets) = OriginalG(indSources);
        TextureB(indTargets) = OriginalB(indSources);

        Texture = cat(3, TextureR, TextureG, TextureB);

        Map = zeros(M,N);
        Map(indTargets) = 1;

        Map = logical(Map);
    
    elseif (strcmpi(interpolation,'BI'))
        
        xSources = dot(bcs(inds,1:3),CPx,2);
        ySources = dot(bcs(inds,1:3),CPy,2);

        xSources(xSources < 1) = 1;
        xSources(xSources > size(OriginalTexture,2)) = size(OriginalTexture,2);

        ySources(ySources < 1) = 1;
        ySources(ySources > size(OriginalTexture,1)) = size(OriginalTexture,1);

        [X,Y] = meshgrid(1:size(OriginalTexture,2),1:size(OriginalTexture,1));
        
        xTargets = qpoints(inds,1);
        yTargets = qpoints(inds,2);
        
        indTargets = sub2ind(Size, yTargets, xTargets);   

        Texture = zeros(M,N,D);
        
        % Go over all of the bands
        for d = 1:D
            
            % now that all the mapings are here need to actually map
            Tex = zeros(M, N);
            
            Z = OriginalTexture(:,:,d);

            % Bilinear interpolation
            ZI = interp2(X,Y, Z, xSources, ySources);
            
            Tex(indTargets) = ZI;
            
            Texture(:,:,d) = Tex;
        end
        
        Map = zeros(M,N);
        Map(indTargets) = 1;

        Map = logical(Map);        
            
    end
end

