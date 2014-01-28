function [ model ] = PolyRegression( X, Y, alpha, lambda )
%PolyRegression Performs polynomial regression using gradient descent -
%quadratic model
%   X is a n-by-m matrix - n is number of training examples, m is number of
%   features

    maxIter = 10000;

    numFeatures = size(X,2);
    numExamples = size(X,1);

    theta0 = 0;
    theta1 = zeros(numFeatures,1);
    theta2 = zeros(numFeatures,1);
    
    % Perform feature scaling
    model.Transform.offset = mean(X);
    model.Transform.scale = std(X);
    X = (X-repmat(model.Transform.offset, numExamples, 1))./repmat(model.Transform.scale, numExamples, 1);
    
    % normalise the parameters - feature scaling
%     model.Transform.offset = min(X, [], 1);
%     X = X - repmat(model.Transform.offset, numExamples, 1);
%     model.Transform.scale = 1./max(X, [], 1);
%     X = X .* repmat(model.Transform.scale, numExamples, 1);
    
    X_sq = X.*X;
    
    cost = CostFunction(theta0, theta1, theta2, X, X_sq, Y, numExamples, lambda);
    costs = zeros(maxIter,1);
    costs(1) = cost;
    
    for iter=1:maxIter
        del_theta0 = 0;
        del_theta1 = zeros(size(theta1));
        del_theta2 = zeros(size(theta2));

        for i=1:numExamples
            error = theta0 + theta1'*X(i,:)' + theta2'*X_sq(i,:)' - Y(i);
            del_theta0 = del_theta0 + error;
            del_theta1 = del_theta1 + (error.*X(i,:))';
            del_theta2 = del_theta2 + (error.*X_sq(i,:))';
        end
        del_theta0 = del_theta0/numExamples;
        del_theta1 = del_theta1/numExamples;
        del_theta2 = del_theta2/numExamples;
        
        theta0 = theta0 - alpha*del_theta0;
        theta1 = theta1 - alpha.*(del_theta1 + lambda/numExamples*theta1);
        theta2 = theta2 - alpha.*(del_theta2 + lambda/numExamples*theta2);
        
        old_cost = cost;
        cost = CostFunction(theta0, theta1, theta2, X, X_sq, Y, numExamples, lambda);
        costs(iter+1) = cost;
        
        % if there has been no change then converged so break
        if (abs(cost-old_cost) < 0.000001)
            break;
        end
        
        if (cost-old_cost) > 0
            break;
        end
    end
    
    model.offset = theta0;
    model.coeffs = [theta1, theta2];
    model.costs = costs(1:iter); %only non-zero costs
    

end

