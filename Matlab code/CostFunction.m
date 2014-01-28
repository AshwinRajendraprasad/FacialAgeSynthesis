function [ cost ] = CostFunction( theta0, theta1, theta2, X, X_sq, Y, numExamples, lambda )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    cost = sum((theta0 + theta1'*X' + theta2'*X_sq' - Y').^2 + lambda*(theta0^2+sum(theta1.^2)+sum(theta2.^2)))/(2*numExamples);
end

