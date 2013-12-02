function [ equal ] = EqualWithinTolerance( vec1, vec2, tolerance )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    if isequal(size(vec1), size(vec2))
        % returns a vector of whether each cell is not equal (not within
        % the tolerance)
        a = abs(vec1 - vec2);
        equalV = abs(vec1 - vec2) > tolerance;
        % only equal if all cells are less than tolerance, i.e. all are not
        % more than the tolerance
        equal = isequal(sum(equalV),0);
    else
        equal = false;
    end
end

