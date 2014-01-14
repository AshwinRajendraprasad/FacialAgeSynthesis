function [ ResizedImage ] = ResizeImage( Image, NewM, NewN )
%RESIZEIMAGE Summary of this function goes here
%   Detailed explanation goes here

    [M, N, B] = size(Image);
    
    ResizedImage = Image;
    
    if(M == NewM && N == NewN)
       return;
    end    

    if(N ~= NewN || M ~= NewM)

        if(M / N ~= NewM / NewN)

            % If the ratios are different need to pad them
            scaleY = NewM / M;
            scaleX = NewN / N;
                
            if (scaleY > scaleX )                            
                scale = scaleX;
                padTop = zeros([uint32((NewM/scale - M) /2) N B]);                
                ResizedImage = cat(1, padTop, Image, padTop);
            else               
                scale = scaleY;
                padLeft = zeros([ M uint32((NewN/scale - N) /2)  B]);                    
                ResizedImage = cat(2, padLeft, Image, padLeft);
            end
        else
            scale = NewM / M;
        end
        % Need to resize the image (either using image pyramids if
        % its a factor 2 difference or just simply resizing if it
        % is not)        
        if(scale < 1)
        
            invS = 1 / scale;
            if( mod(invS, 2) == 0)
               
                % can use gaussian pyramids for rescaling
                amount = invS / 2;                
                for i=1:amount
                    ResizedImage = impyramid(ResizedImage, 'reduce');                
                end
            else
               ResizedImage = imresize(ResizedImage, scale, 'bilinear'); 
            end                        
        elseif(scale > 1)
            
            if( mod(scale,2) == 0 )
                amount = scale / 2;
                for i=1:amount
                    ResizedImage = impyramid(ResizedImage, 'expand');
                end
            else
                ResizedImage = imresize(ResizedImage, scale, 'bilinear'); 
            end
                
        end

    end

end

