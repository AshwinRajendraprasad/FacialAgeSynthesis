function DisplayAppearanceVariation( AppearanceModel, numModes, numExamples, mask, numChannels )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

    % from -3 to 3 sqrt(var)
    inc = 6 / (numExamples - 1);

    for i=1:numModes
        
        for j=1:numExamples
            
            currPlot = subplot(numModes, numExamples, (i-1)*numExamples + j);
            % get the shape

            params = zeros(size(AppearanceModel.variances));
            params(i) = sqrt(AppearanceModel.variances(i))*(-3 + inc*(j-1));
            
            texture = AppearanceParams2Texture(params, AppearanceModel);

            subimage(AddZerosToImage(mask, texture, numChannels)/255);

            axis(currPlot, 'off');

        end

    end
end

