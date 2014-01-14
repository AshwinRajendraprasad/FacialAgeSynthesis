function GraphConvergenceBasedOnRadius2(dispX, dispY, Converged, dispX2, dispY2, Converged2)
%GRAPHCONVERGENCEBASEDONRADIUS Works out how many possible radii there are and then plots appropriate
% convergence graphs
    
    radii = [];
    
    radiiMap = zeros(numel(dispX),1);
    
    % Find the possible Radii and sort them in ascendin order
    for i = 1:numel(dispX)
        radius = sqrt(dispX(i)^2 + dispY(i)^2);
        if(numel(find(radii==radius)) == 0)
            radii = [radii radius];
        end
        radiiMap(i) = radius;
    end
    
    RadiusConv = zeros(numel(radii),2);
    
    radii = sort(radii);
    
    for i = 1:size(Converged,1)
        for j = 1:numel(dispX)
            currRadius = radiiMap(j);
            ind = find(radii==currRadius,1);
            if(Converged(i,j))
                RadiusConv(ind,1) = RadiusConv(ind,1) + 1;
            end
            RadiusConv(ind,2) = RadiusConv(ind,2) + 1;
        end
    end
    
    radii2 = [];
    
    radiiMap2 = zeros(numel(dispX2),1);
    
    % Find the possible Radii and sort them in ascendin order
    for i = 1:numel(dispX2)
        radius2 = sqrt(dispX2(i)^2 + dispY2(i)^2);
        if(numel(find(radii2==radius2)) == 0)
            radii2 = [radii2 radius2];
        end
        radiiMap2(i) = radius2;
    end
    
    RadiusConv2 = zeros(numel(radii2),2);
    
    radii2 = sort(radii2);
    
    for i = 1:size(Converged2,1)
        for j = 1:numel(dispX2)
            currRadius2 = radiiMap2(j);
            ind2 = find(radii2==currRadius2,1);
            if(Converged2(i,j))
                RadiusConv2(ind2,1) = RadiusConv2(ind2,1) + 1;
            end
            RadiusConv2(ind2,2) = RadiusConv2(ind2,2) + 1;
        end
    end    
    
    X = radii;
    Y = RadiusConv(:,1) ./ RadiusConv(:,2);
  
    X2 = radii2;
    Y2 = RadiusConv2(:,1) ./ RadiusConv2(:,2);    
    
    X3 = [X X2];
    [X3, ind3] = sort(X3);
    Y3 = [ Y; Y2];
    Y3 = Y3(ind3);
    
    plot(X3, Y3);
    
    %title('Rate of convergence based on radius');
    xlabel('Radius');
    ylabel('Rate of convergence');

end