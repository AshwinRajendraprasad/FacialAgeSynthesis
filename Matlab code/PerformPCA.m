function [ modes, variances ] = PerformPCA( textures, proportion_explained )
    
    % Make the mean of each pixel 0 for the PCA
    textures = textures - repmat(mean(textures,1),size(textures,1),1);
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

    [modes, ~, variances] = pca(textures, 'Economy', true);

    t = FindT(variances, proportion_explained);
    modes = modes(:,1:t);
    variances = variances(1:t,:);
end