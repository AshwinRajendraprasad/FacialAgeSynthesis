function [MeanTexture, NormalisedTextures, Transform] = NormaliseTextures(Textures, NumberBands, BandNormalisation)

    if(strcmpi(BandNormalisation, 'Global'))

        % Get the overall mean texture
        meanTexture = mean(Textures,2);

        % normalise the mean texture
        NormalisedTextures = Textures;

        %Transform.TranslateLocal = mean(NormalisedTextures,1);    

        Transform.TranslateGlobal = mean(meanTexture);
        meanTexture = meanTexture - mean(meanTexture);
        Transform.ScaleGlobal = std(meanTexture);
        meanTexture = meanTexture ./ std(meanTexture);

        %Transform.ScaleLocal = ones(size(NormalisedTextures,2),1);

        % iterate over all textures normalising them to the mean until
        % mean becomes stable (or reached number of max iterations)

        maxIter = 10;

        for i=1:maxIter

            % transform all textures to 0 mean
            NormalisedTextures(:,:) = NormalisedTextures(:,:) - repmat(mean(NormalisedTextures,1),size(NormalisedTextures,1),1);

            % align all texture to mean texture
            for j=1:size(NormalisedTextures,2)
                %Transform.ScaleLocal(j) = Transform.ScaleLocal(j) * (dot(meanTexture,NormalisedTextures(:,j))/numel(meanTexture));
                NormalisedTextures(:,j) = NormalisedTextures(:,j) ./ (dot(meanTexture,NormalisedTextures(:,j))/numel(meanTexture));
            end

            oldMean = meanTexture;

            % recalculate the mean
            meanTexture = mean(NormalisedTextures,2);

            meanTexture = meanTexture - mean(meanTexture);
            meanTexture = meanTexture ./ std(meanTexture);

            res = norm(meanTexture - oldMean,'fro');

            if(res < 1)
               break; 
            end

        end

        % now scale all of the textures to numerically reasonable values (e.g.
        % so that std of all of them would be 1)
        meanTexture = mean(NormalisedTextures,2);
        MeanTexture = meanTexture;
        NormalisedTextures(:,:) = NormalisedTextures(:,:) ./ std(meanTexture);
% 
%         for j=1:size(NormalisedTextures,2)
%             Transform.ScaleLocal(j) = Transform.ScaleLocal(j) * std(meanTexture);
%         end
    else

        sBandLength = numel(Textures(:,1)) / NumberBands;
        
        NormalisedTextures = Textures;
        
        MeanTexture = zeros(sBandLength, NumberBands);
        
        for d = 1:NumberBands
                        
            % Get the mean texture for the currentBand
            st = sBandLength*(d-1)+1;
            en = sBandLength*d;
            
            meanTexture = mean(Textures(st:en,:),2);

            Transform.TranslateGlobal(d) = mean(meanTexture);
            meanTexture = meanTexture - mean(meanTexture);
            Transform.ScaleGlobal(d) = std(meanTexture);
            meanTexture = meanTexture ./ std(meanTexture);            
            
            % iterate over all textures normalising them to the mean until
            % mean becomes stable (or reached number of max iterations)
            maxIter = 10;

            for i=1:maxIter

                % transform all textures to 0 mean
                
                NormalisedTextures(st:en,:) = NormalisedTextures(st:en,:) - repmat(mean(NormalisedTextures(st:en,:),1),sBandLength,1);

                % align all texture to mean texture
                for j=1:size(NormalisedTextures,2)
                    NormalisedTextures(st:en,j) = NormalisedTextures(st:en,j) ./ (dot(meanTexture,NormalisedTextures(st:en,j))/numel(meanTexture));
                end

                oldMean = meanTexture;

                % recalculate the mean
                meanTexture = mean(NormalisedTextures(st:en,:),2);


                
                meanTexture = meanTexture - mean(meanTexture);
                meanTexture = meanTexture ./ std(meanTexture);

                res = norm(meanTexture - oldMean,'fro');

                if(res < 1)
                   break; 
                end

            end

            % now scale all of the textures to numerically reasonable values (e.g.
            % so that std of all of them would be 1)
            meanTexture = mean(NormalisedTextures(st:en,:),2);            
            
            MeanTexture(:, d) = meanTexture;

            NormalisedTextures(st:en,:) = NormalisedTextures(st:en,:) ./ std(meanTexture);
        end
    end
end