function GraphConvergenceBasedOnRadius(dispX, dispY, Converged)
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
    
    X = radii;
    Y = RadiusConv(:,1) ./ RadiusConv(:,2);
    
    plot(X, Y);
    
    title('Rate of convergence based on radius');
    xlabel('Radius');
    ylabel('Rate of convergence');

end

