function [ b ] = FindModelParameters( modes_inv, gbar, g )
%FindModelParameters Uses the inverse of the model to find the model
%parameters for this image

    b = ((gbar - g)*transpose(modes_inv))';
end

