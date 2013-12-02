function [gbar] = FindSimpleMean(allWarped)
    gbar = zeros(1,size(allWarped,2));
    for i=1:size(allWarped,1)
        gbar = gbar + squeeze(allWarped(i,:));
    end
    gbar = gbar / size(allWarped,1);
end