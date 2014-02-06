function [ age ] = PredictAge( AgeingModel, b )
%PredictAge Does what it says in the title
%   Detailed explanation goes here

    b = (b - AgeingModel.Transform.offset') ./ AgeingModel.Transform.scale';
    age = AgeingModel.offset + AgeingModel.coeffs(:,1)'*b + AgeingModel.coeffs(:,2)'*(b.*b);

%     age = predict(AgeingModel.mdl, [b ; b.^2]');
end

