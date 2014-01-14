function DisplayNormalisedShapes( TrainingData, normX, normY, meanShape )
%DISPLAYNORMALISEDSHAPES Display both normalised and non-normalised shapes
%   Detailed explanation goes here

    figure;

    % Display the non-normalised data
    subplot(2,1,1);
    
    for i=1:TrainingData.NumExamples        
        for j=1:max(TrainingData.paths) + 1
            % connecting the lines based on the path, assume they're
            % ordered    
            toConnectFirst = find(TrainingData.paths == j - 1, 1, 'first');
            toConnectLast = find(TrainingData.paths == j - 1, 1, 'last');
            connectX = TrainingData.xVals(i, toConnectFirst : toConnectLast);
            connectY = TrainingData.yVals(i, toConnectFirst : toConnectLast);

            % type 4 is an open path, while 0 is closed
            if(TrainingData.types(toConnectFirst) == 4)
                line(connectX, connectY, 'Marker', 'x');                
            else
                line([connectX connectX(1)], [connectY connectY(1)], 'Marker', 'x');
            end                
        end 
    end    
    
    % Display normalised data
    subplot(2,1,2);

    for i=1:TrainingData.NumExamples     
        for j=1:max(TrainingData.paths) + 1
            % connecting the lines based on the path, assume they're
            % ordered    
            toConnectFirst = find(TrainingData.paths == j - 1, 1, 'first');
            toConnectLast = find(TrainingData.paths == j - 1, 1, 'last');
            connectX = normX(i, toConnectFirst : toConnectLast);
            connectY = normY(i, toConnectFirst : toConnectLast);

            % type 4 is an open path, while 0 is closed
            if(TrainingData.types(toConnectFirst) == 4)
                line(connectX, connectY, 'Marker', 'x');                
            else
                line([connectX connectX(1)], [connectY connectY(1)], 'Marker', 'x');
            end                
        end 
    end

    % draw a mean shape in diff color
    for j=1:max(TrainingData.paths) + 1
        % connecting the lines based on the path, assume they're
        % ordered    
        toConnectFirst = find(TrainingData.paths == j - 1, 1, 'first');
        toConnectLast = find(TrainingData.paths == j - 1, 1, 'last');
        meanX = meanShape(:,1)';
        meanY = meanShape(:,2)';
        connectX = meanX(toConnectFirst : toConnectLast);
        connectY = meanY(toConnectFirst : toConnectLast);

        if(TrainingData.types(toConnectFirst) == 4)
            line(connectX, connectY, 'Marker', 'x','Color', 'red', 'LineWidth',3);                
        else
            line([connectX connectX(1)], [connectY connectY(1)], 'Marker', 'x','Color', 'red', 'LineWidth',3);
        end                
    end     

end

