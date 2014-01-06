function [gbar, aligned, average_alpha, average_beta] = FindMean(allWarped)
    maxIter = 1000;
    alignment = zeros(size(allWarped,1), 2);
    aligned = allWarped;
    % Start by choosing any sample as the mean (choose the first)
    gbar = allWarped(1,:);

    % Create vector the size of the images that is all ones, used for beta
    allOnes = ones(size(gbar));

    gbar_old = zeros(size(gbar));
    aligned_gbar = ScaleAndOffset(gbar);
    
    % The number of elements in a texture vector
    n = size(gbar,2);
    j = 0;
    while not(EqualWithinTolerance(gbar, gbar_old, 0.00000001))
        j = j+1;
        if j > maxIter
            break;
        end
        for i=1:size(allWarped,1)
            g = allWarped(i,:);
            %alpha
            alignment(i,1) = dot(g, aligned_gbar);
            %beta
            alignment(i,2) = dot(g, allOnes)/n;
            g = (g-alignment(i,2)*allOnes)/alignment(i,1);
            aligned(i,:) = g;
        end
        gbar_old = gbar;
        gbar = FindSimpleMean(allWarped);
        aligned_gbar = ScaleAndOffset(gbar);
    end
    
    % Return the average of these to return the model back to original
    % values before adding to the mean to give an instantiation
    average_alpha = mean(alignment(:,1));
    average_beta = mean(alignment(:,2));
end