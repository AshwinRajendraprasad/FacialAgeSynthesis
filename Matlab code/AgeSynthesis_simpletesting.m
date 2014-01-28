i=round(rand(1)*5504);
age_change = round(rand(1)*40-20);

b = FindModelParameters(AppearanceModel, allWarped1(i,:));
subplot(1,2,1), subimage(AddZerosToImage(mask, AppearanceParams2Texture(b, AppearanceModel))/255);

b_new = ChangeFaceAge(AgeingModel_general, AppearanceModel, allWarped1(i,:), inverse_ageing, age_change);
subplot(1,2,2), subimage(AddZerosToImage(mask, AppearanceParams2Texture(b_new, AppearanceModel))/255);

age_est_new = round(PredictAge(AgeingModel_general, b_new));
display(age_est_new);