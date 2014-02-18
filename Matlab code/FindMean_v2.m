function [mean_texture, aligned_textures, Transform] = FindMean_v2(textures, single_iter)
    maxIter = 10;
    if single_iter
        maxIter = 1;
    end

    aligned_textures = textures;
    
    mean_texture = mean(textures, 1);

    Transform.translate = mean(mean_texture);
    mean_texture = mean_texture - Transform.translate;
    Transform.scale = std(mean_texture);
    mean_texture = mean_texture / Transform.scale;
    
    
    % The number of elements in a texture vector
    n = size(mean_texture,2);

    % align the textures to the mean in an iterative process until the mean
    % doesn't change (convergence) or the maximum number of iterations has
    % been reached
    for j=1:maxIter
        % transform textures to 0 mean
%         aligned_textures = aligned_textures - repmat(mean(aligned_textures,2),1,size(aligned_textures,2));
%         for i=1:size(textures,1)
%             aligned_textures(i,:) = aligned_textures(i,:) ./ (dot(mean_texture, aligned_textures(i,:))/n);
%         end
        
        aligned_textures = textures - repmat(mean(textures,2),1,size(textures,2));
        for i=1:size(textures,1)
            aligned_textures(i,:) = aligned_textures(i,:) ./ (dot(mean_texture, textures(i,:))/n);
        end

        mean_texture_old = mean_texture;
        % recalculate and normalise mean
        mean_texture = mean(aligned_textures);
        mean_texture = mean_texture - mean(mean_texture);
        mean_texture = mean_texture / std(mean_texture);

        % if there has been no change then converged so break
        if (norm(mean_texture - mean_texture_old,'fro') < 1)
            break;
        end
    end
    
    % scale textures so std dev is 1
    aligned_textures = aligned_textures / std(mean_texture);
end