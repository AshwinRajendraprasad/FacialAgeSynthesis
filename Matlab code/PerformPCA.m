function [ eigenvectors, eigenvalues, gbar ] = PerformPCA( allWarped, proportion )
     [gbar, aligned] = FindMean(allWarped);
     
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

    [eigenvectors, ~, eigenvalues] = pca(aligned, 'Algorithm', 'eig');

%     t = FindT(eigenvalues, proportion);
%     eigenvectors = eigenvectors(:,1:t);
%     eigenvalues = eigenvalues(1:t,:);
end