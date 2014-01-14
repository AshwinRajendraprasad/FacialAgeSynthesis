% data is ordered by columns whith each column being an observation
% and rows being variables, input is the data and what amount of variance
% explained is required
function [ PrincipalComponents, Variances ] = PrincipalComponentAnalysis( Data, VarianceExplained )

    % move the data to 0-mean in each dimension in order to construct the
    % covariance matrix
    
    [M,N] = size(Data);
    
    mn = mean(Data,2);
    Data = Data - repmat(mn,1, N);
    
    %covariance = 1 / (N-1) * (Data * Data');
% 
%     [PrincipalComponents, Variances] = eig(covariance);
%     
%     % extract the matrix as a vector
%     Variances = diag(Variances);
%     
%     % sort the variances in decreasing order
%     [Variances, indices] = sort(Variances,'descend');
%     PrincipalComponents = PrincipalComponents(:,indices);
%     
    [PrincipalComponents, ~, Variances,] = princomp(Data','econ');

    % keep only the necessary variance
    sumVar = sum(Variances);
    sumCurr = 0;
    for i = 1:size(Variances)
        sumCurr = sumCurr + Variances(i)/sumVar;
        if(sumCurr > VarianceExplained)
           Variances = Variances(1:i); 
           PrincipalComponents = PrincipalComponents(:,1:i);
           break;
        end
    end
    
end

