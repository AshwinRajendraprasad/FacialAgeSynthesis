function GraphErrorBasedOnRadius(dispX, dispY, Converged, ErrorPtC, plotOnlyConverged)
%GRAPHERRORBASEDONRADIUS Graphing the error of the model fitting based on
% the displacement radius

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
    
    RadiusPtC = cell(numel(radii),1);
    
    radii = sort(radii);
    
    for i = 1:size(ErrorPtC,1)
        for j = 1:numel(dispX)
            currRadius = radiiMap(j);
            ind = find(radii==currRadius,1);
            if(plotOnlyConverged && Converged(i,j))
                RadiusPtC{ind} = [RadiusPtC{ind} ErrorPtC(i,j)];
            elseif(~plotOnlyConverged)
                RadiusPtC{ind} = [RadiusPtC{ind} ErrorPtC(i,j)];
            end
        end
    end
    
    figure;
    
    X = radii;
    Y = zeros(numel(radii),1);
    err = zeros(numel(radii),1);
    
    for i = 1:numel(radii)
        Y(i) = mean(RadiusPtC{i});
        err(i) = std(RadiusPtC{i});
    end
    
    errorbar(X, Y,  err);

    title('Point to curve error based on radius');
    xlabel('Radius');
    ylabel('Point to curve error');    
    
end

