function [ tri ] = FindTriangulationPoints( triNum, points )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
    sizeTri = size(triNum);
    tri = zeros(sizeTri(1),6);
    
    for i = 1:(sizeTri(1))
        tri1 = findPointsInOne( triNum(i,:), points );
        tri(i,:) = tri1;
    end
end

function [ tri1 ] = findPointsInOne( triNum1, points1)

    %Add one to each as they are zero based in the triangulation but arrays
    %are 1 based
    t1 = points1(triNum1(1)+1,:);
    t2 = points1(triNum1(2)+1,:);
    t3 = points1(triNum1(3)+1,:);
    tri1 = [t1 t2 t3];

end

