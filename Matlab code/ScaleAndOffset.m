function [scaled_gbar] = ScaleAndOffset(gbar)
    % offset by taking off the mean from all points, now sum of elements is
    % zero
    average = mean(gbar);
    scaled_gbar = gbar - average;
    % scale by dividing by standard deviation, now standard deviation is
    % unity
    stddev = std(gbar);
    scaled_gbar = scaled_gbar / stddev;
end