function [ WarpMatrix ] = GetWarpMatrix( xi0, yi0, xj0, yj0, xk0, yk0, xi1, yi1, xj1, yj1, xk1, yk1 )
%GETWARPMATRIX Produces a piecewise affine warp based on the control points

    % eq 25-27 in AAM Revisited
    c = (xj0 - xi0) * (yk0 - yi0) - (yj0 - yi0) * (xk0 - xi0);

    a11 = ((yk0 - yi0) * (xj1 - xi1) - (yj0 - yi0) * (xk1 - xi1)) / c;
    a12 = ((xj0 - xi0) * (xk1 - xi1) - (xk0 - xi0) * (xj1 - xi1)) / c;
    a13 = ((yi0 * xk0 - xi0 * yk0) * (xj1 - xi1) + (xi0 * yj0 - yi0 * xj0) * ( xk1 - xi1 )) / c + xi1;
    a21 = ((yk0 - yi0) * (yj1 - yi1) - (yj0 - yi0) * (yk1 - yi1)) / c;
    a22 = ((xj0 - xi0) * (yk1 - yi1) - (xk0 - xi0) * (yj1 - yi1)) / c;
    a23 = ((yi0 * xk0 - xi0 * yk0) * (yj1 - yi1) + (xi0 * yj0 - yi0 * xj0) * ( yk1 - yi1 )) / c + yi1;
    
    WarpMatrix = [a11 a12 a13; a21 a22 a23];
    
end