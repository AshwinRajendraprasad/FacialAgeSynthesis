function [ V, D, gbar, avg_alpha ] = PerformPCA( allWarped, proportion )

    [gbar, aligned, avg_alpha, ~] = FindMean(allWarped);
     
%     % Calculate the covariance matrix
%     S = 0;
%     for i=1:size(aligned,1)
%        gi = aligned(i,:);
%        S = S + (gi - gbar) * transpose(gi - gbar);
%     end
%     S = S / (size(aligned,1)-1);
% 
%     % performs PCA on the covariance of the data, used as I want to be able
%     % to calculate the mean myself
%     [eigenvectors, eigenvalues] = pcacov(S);

    [V, ~, D] = pca(aligned, 'Economy', true);

    t = FindT(D, proportion);
    V = V(:,1:t);
    D = D(1:t,:);
end