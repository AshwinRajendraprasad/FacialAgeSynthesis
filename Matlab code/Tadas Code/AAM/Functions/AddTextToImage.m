function [ Image ] = AddTextToImage( textToAdd, Image, x, y )
%ADDTEXTTOIMAGE Summary of this function goes here
%   Detailed explanation goes here

    % Create and style the text in an axis:
    
%     f = figure('Position', [0 0 size(Image,2) + 100 size(Image,1) + 100]);
%     text(x,y, textToAdd, 'FontSize',12);
%     % Capture the text from the screen:
%     %F = getframe(gca,[0 0 size(Image,1) size(Image,2)]);
%     F = getframe(gca, [10 10 size(Image,2) size(Image,1)]);
%     % Close the figure:
%     close(f);
% 
%     % Select any plane of the resulting RGB image:
%     c = F.cdata(:,:,1);
% 
%     % Note: If you have the Image Processing Toolbox installed,
%     % you can convert the RGB data from the frame to black or white:
%     % c = rgb2ind(F.cdata,2);
% 
%     % Determine where the text was (black is 0):
%     [i,j] = find(c == 0);
%         
%     % Use the size of that image, plus the row/column locations
%     % of the text, to determine locations in the new image:
%     ind = sub2ind([size(Image,1), size(Image,2)], i ,j);
% 
%     % For colour images
%     if(ndims(Image) == 3)
%         R = Image(:,:,1);
%         G = Image(:,:,2);
%         B = Image(:,:,3);
%         R(ind) = 1;
%         G(ind) = 1;
%         B(ind) = 1;
%         Image = cat(3,R,G,B);
%     else
%         Image(ind) = 1;
%     end

% Create the text mask
% Make an image the same size and put text in it
hf = figure('color','white','units','normalized','position',[.1 .1 .8 .8]);
image(ones(size(Image)));
set(gca,'units','pixels','position',[5 5 size(Image,2)-1 size(Image,1)-1],'visible','off')

% Text at arbitrary position
text('units','pixels','position',[x y],'fontsize',30,'string',textToAdd)

% Capture the text image
% Note that the size will have changed by about 1 pixel
tim = getframe(gca);
close(hf)

% Extract the cdata
tim2 = tim.cdata;

% Make a mask with the negative of the text
if(size(Image,3) == 1)
    tmask = tim2(:,:,3)==0;
else
    tmask = tim2==0;
end
Image(tmask) = 1;

end

