function [ texture ] = AppearanceParams2Texture( b, AppearanceModel )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    texture = (AppearanceModel.mean_texture' + AppearanceModel.modes * b) * AppearanceModel.Transform.scale +  AppearanceModel.Transform.translate;

end

