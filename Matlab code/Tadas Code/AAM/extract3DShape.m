% read the point location
function extract3DShape(radius)

if(nargin == 0)
    radius = 30;
end

location = 'E:\Databases\3dImg\';

subjects = dir(location);

%minZ = 0;
%maxZ = 0;

for i = 1:numel(subjects)

    if(~(strcmp(subjects(i).name,'.') || strcmp(subjects(i).name,'..')))
       
        emotions = dir([location subjects(i).name]);
        
        for r = 1:numel(emotions)
            if(~(strcmp(emotions(r).name,'.') || strcmp(emotions(r).name,'..')))

                % get all the .lbl and .wrl files
                landmarkLabels = dir([location subjects(i).name '\' emotions(r).name '\*.lbl']);
                wrlFiles = dir([location subjects(i).name '\' emotions(r).name '\*.wrl']);
                imgFiles = dir([location subjects(i).name '\' emotions(r).name '\*.jpg']);
                
                for j=1:numel(landmarkLabels)
                   
                    [pointCloud, texCoords] = readwrlFile([location subjects(i).name '\' emotions(r).name '\' wrlFiles(j).name]);
                    
                    labels = readlblFile([location subjects(i).name '\' emotions(r).name '\' landmarkLabels(j).name]);
                    
                    [~, name, ~] = fileparts(landmarkLabels(j).name);
                    
                    % map the labels to the point cloud
                    
                    [h xout] = hist(pointCloud(:,3), 200);
                    [~, ind] = max(h);
                    faceDepth = xout(ind);
                    
                    % center the face at 50
                    pointCloud(:,3) = -pointCloud(:,3) + faceDepth + 50;

                    % get the points within the radius of labels
                    %xs = repmat(pointCloud(:,1), 1, size(labels,1)) - repmat(labels(:,1), 1, size(pointCloud,1))';
                    %ys = repmat(pointCloud(:,3), 1, size(labels,1)) - repmat(labels(:,2), 1, size(pointCloud,1))';
                    xs = repmat(pointCloud(:,1), 1, size(labels,1));
                    ys = repmat(pointCloud(:,2), 1, size(labels,1));
                    zs = repmat(pointCloud(:,3), 1, size(labels,1));
                    
                    % need to find the mode of depth and push it to a
                    % certain point
                    
                    %minZ = min(minZ, min(pointCloud(:,3)));
                    %maxZ = max(maxZ, max(pointCloud(:,3)));
                    
                    % need to get image size
                    [img] = imread([location subjects(i).name '\' emotions(r).name '\' imgFiles(j).name]);
                    [height, width,~] = size(img);
                    
                    labels(:,1) = width - labels(:,1);
                    
                    % x's and y's in the image frame
                    xImg = repmat(texCoords(:,1) * width, 1, size(labels,1)) - repmat(labels(:,1), 1, size(texCoords,1))';
                    yImg = repmat(height - texCoords(:,2) * height, 1, size(labels,1)) - repmat(labels(:,2), 1, size(texCoords,1))';                    
                    
                    localPoints = sqrt(xImg.^2 + yImg.^2) < radius;
                    for t = 1:size(labels,1)
                        x(t) = mean(xs(localPoints(:,t)));
                        y(t) = mean(ys(localPoints(:,t)));
                        z(t) = mean(zs(localPoints(:,t)));
                    end
                    writelbl3dFile([location subjects(i).name emotions(r).name '\' landmarkLabels(j).name '3d'], imgFiles(j).name, imgFiles(j).name, [x; y; z]', labels(:,1), labels(:,2));
                    writeDispFile([location subjects(i).name emotions(r).name '\' name 'd.png'], texCoords, pointCloud, width, height, 10, 150);
                end
                
            end
            fprintf('emotion %d done\n', r);
        end
        fprintf('subject %d done\n', i);
    end
    
end
end

% read the bhamton style wrl file
function [pointCloud, texCoords] = readwrlFile(location)

    fid = fopen(location);
    %pointCloud = textscan(fid, '%f %f %f\n', 'HeaderLines', 24, 'ReturnOnError', true);
    tmpCloud = textscan(fid, '%f %f %f,', 'HeaderLines', 24);
    
    pointCloud(:,1) = tmpCloud{1};
    pointCloud(:,2) = tmpCloud{2};
    pointCloud(:,3) = tmpCloud{3};
    
    tmpTex = textscan(fid, '%f %f,', 'HeaderLines', 3);
    
    texCoords(:,1) = tmpTex{1};
    texCoords(:,2) = tmpTex{2};
    
    fclose(fid);

end

% read the bhamton style wrl file
function [labels] = readlblFile(location)

    fid = fopen(location);
    %pointCloud = textscan(fid, '%f %f %f\n', 'HeaderLines', 24, 'ReturnOnError', true);
    tmpLabels = textscan(fid, '%f %f');
    
    labels(:,1) = tmpLabels{1};
    labels(:,2) = tmpLabels{2};
    
    fclose(fid);

end

% write the labeled file (both 3d and tex coordinates)
function writelbl3dFile(location, texLoc, depthLoc, verts, x2, y2)
   
    labelFile = fopen(location, 'w');

    fprintf(labelFile, '# Labeled shape file \n');
    fprintf(labelFile, '# \n');
    fprintf(labelFile, '# number of vertices \n');
    fprintf(labelFile, '%d\n', size(verts, 1));
    fprintf(labelFile, '# texture file\n');
    fprintf(labelFile, '%s\n', texLoc);
    fprintf(labelFile, '# depth image\n');
    fprintf(labelFile, '%s\n', depthLoc);                
    fprintf(labelFile, '# texture coordinates\n');
    fprintf(labelFile, '%f,%f\n', cat(2, x2, y2)');

    fprintf(labelFile, '# shape vertices\n');
    fprintf(labelFile, '%f,%f,%f\n', verts');

    %fprintf(labelFile, '#Camera matrix\n');
    %fprintf(labelFile, '%f,%f,%f,%f\n', A);

    %fprintf(labelFile, '#Triangle mesh location\n');
    %fprintf(labelFile, 'pyrTri.txt\n');

    fclose(labelFile);

end

function writeDispFile(location, texCoords, pointCloud, width, height, minD, maxD)

    w = round(width/6);
    h = round(height/6);
    depthImg = maxD * ones(w, h);

    dispP = 2^11;
    
    %radius = 10;
    %h = [1:height]';
    %z = zeros(height,1);
    %zs = repmat(pointCloud(:,2), 1, size(h,1));
%     for x = 1:width
%                 
%         % x's and y's in the image frame
%         xImg = repmat(texCoords(:,1) * width, 1, size(h,1)) - repmat(h, 1, size(texCoords,1))';
%         yImg = repmat(height - texCoords(:,2) * height, 1, size(h,1)) - repmat(h, 1, size(texCoords,1))';                    
% 
%         localPoints = sqrt(xImg.^2 + yImg.^2) < radius;
%         for t = 1:size(h,1)
%             z(t) = mean(zs(localPoints(:,t)));
%         end
%         dispImg(x,:) = z;
% %             xImg = texCoords(:,1) * width - repmat(x, 1, size(texCoords,1))';
% %             yImg = (height - texCoords(:,2) * height) - repmat(y, 1, size(texCoords,1))';
% %             localPoints = sqrt(xImg.^2 + yImg.^2) < radius;        
% %             if(sum(localPoints) > 0)
% %                 dispImg(x,y) = mean(zs(localPoints));
% %             end
%         fprintf('%d scanline done\n', x);
%     end
    
            
    xLoc = round(texCoords(:,1) * w);
    yLoc = round(h - texCoords(:,2) * h);
    for i = 1:numel(xLoc)
        depthImg(xLoc(i), yLoc(i)) = pointCloud(i,3);        
    end
    
    depthImg(depthImg < minD) = minD;
    depthImg(depthImg > maxD) = maxD;
    
    diff = maxD - minD;
    
    %dispImg = uint16((dispP./((dispImg - minD + 1))));
    
    % kinect disp
    %dispImg = uint16(-325*(diff./dispImg - 0.33));
    dispImg = uint16((dispP / (1/minD - 1/maxD)) * ((1./depthImg) - 1/maxD));
    dispImg = dispImg';
    dispImg = imresize(dispImg, [height width]);
    imwrite(dispImg, location, 'png', 'bitdepth', 16, 'SignificantBits', 11);
    %linearInd = sub2ind(size(dispImg), xLoc, yLoc);
    %dispImg(linearInd) = pointCloud(:,2);
end