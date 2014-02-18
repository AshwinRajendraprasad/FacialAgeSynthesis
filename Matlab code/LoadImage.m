function texture = LoadImage(path, im_name, pts_name, M, T, numChannels)
    im = ImportImage(strcat(path, '\', im_name));
    points = ImportLandmarks(strcat(path, '\', pts_name));
    
    texture = WarpSingleImage(im, points, M, T, numChannels)';
end