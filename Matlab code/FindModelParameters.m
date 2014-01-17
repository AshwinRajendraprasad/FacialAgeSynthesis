function [ b ] = FindModelParameters( AppearanceModel, image )
%FindModelParameters Uses the inverse of the model to find the model
%parameters for this image

    b = AppearanceModel.modes' * ((image - AppearanceModel.Transform.translate)/AppearanceModel.Transform.scale - AppearanceModel.mean_texture)';
end

