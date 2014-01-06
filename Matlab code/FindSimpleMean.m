function [gbar] = FindSimpleMean(allWarped)
%     gbar = zeros(1,size(allWarped,2));
%     for i=1:size(allWarped,1)
%         gbar = gbar + squeeze(allWarped(i,:));
%     end
%     gbar = gbar / size(allWarped,1);

    % simpler representation than above, and possibly faster as uses
    % internal functions
    gbar = sum(allWarped, 1) / size(allWarped, 1);
end