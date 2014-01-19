function [ model ] = LinearRegression( X, Y, alpha )
%LinearRegression Summary of this function goes here
%   X is a n-by-m matrix - n is number of training examples, m is number of
%   features

    maxIter = 5000;

    numFeatures = size(X,2);
    numExamples = size(X,1);

%     % Perform feature scaling
%     model.offset = mean(X);
%     model.scale = std(X);
%     X = (X-repmat(model.offset, numExamples, 1))./repmat(model.scale, numExamples, 1);
    
    % normalise the parameters - feature scaling
    model.Transform.offset = min(X, [], 1);
    X = X - repmat(model.Transform.offset, numExamples, 1);
    model.Transform.scale = 1./max(X, [], 1);
    X = X .* repmat(model.Transform.scale, numExamples, 1);
    
    theta0 = 0;
    theta1 = zeros(numFeatures,1);
    
    cost = sum((theta0 + theta1'*X' - Y').^2)/(2*numExamples);
    costs = zeros(maxIter,1);
    costs(1) = cost;
    
    for iter=1:maxIter
        del_theta0 = 0;
        del_theta1 = zeros(size(theta1));

        for i=1:numExamples
            error = theta0 + theta1'*X(i,:)' - Y(i);
            del_theta0 = del_theta0 + error;
            del_theta1 = del_theta1 + (error.*X(i,:))';
        end
        del_theta0 = del_theta0/numExamples;
        del_theta1 = del_theta1/numExamples;
        
        theta0 = theta0 - alpha.*del_theta0;
        theta1 = theta1 - alpha.*del_theta1;
        
        old_cost = cost;
        cost = sum((theta0 + theta1'*X' - Y').^2)/(2*numExamples);
        costs(iter+1) = cost;
        
        % if there has been no change then converged so break
        if (abs(cost-old_cost) < 0.00000000001)
            break;
        end
    end
    
    model.offset = theta0;
    model.coeffs = theta1;
    model.costs = costs(1:iter); %only non-zero costs
    

end

