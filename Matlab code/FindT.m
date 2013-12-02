% Given descending sorted values, and a proportion of total (<1) that want
% covered, return the cut off point
function [ t ] = FindT(eigenvalues, proportion)
    target = sum(eigenvalues) * proportion;
    cumulativeSum = cumsum(eigenvalues);
    for i=1:size(cumulativeSum,1)
        t=i;
        if (cumulativeSum(i) >= target)
            break
        end
    end
end