%%
AppearanceModel = BuildAppearanceModel(train.textures, false);

app_avgErr = AppearanceModelAvgErr(AppearanceModel, test.textures);

% Use cross validation to find the minimum lambda, then build model using
min_lam = CrossValidation_ageEst(train.textures, train.numbers, subjectlist, AppearanceModel);
AgeingModel = BuildAgeingModel_reg_singlelam(train.textures, AppearanceModel, subjectlist, train.numbers, min_lam);

ageEst_err = TestAgeingModel(AgeingModel, AppearanceModel, test.textures, test.numbers, subjectlist);
ageEst_avgErr = mean(abs(ageEst_err(:, 2)));

clear AppearanceModel AgeingModel

%%
[AppModelGen.Male, AppModelGen.Female] = BuildAppearanceModel_Gender(train.textures, subjectlist, genders, train.numbers);
[male_test, female_test, male_test_inds, female_test_inds] = SplitTextures_Gender(test.textures, subjectlist, genders, test.numbers);
appM_avgErr = AppearanceModelAvgErr(AppModelGen.Male, male_test);
appF_avgErr = AppearanceModelAvgErr(AppModelGen.Female, female_test);

[male_train, female_train, male_train_inds, female_train_inds] = SplitTextures_Gender(train.textures, subjectlist, genders, train.numbers);

min_lam_m = CrossValidation_ageEst(male_train, train.numbers(male_train_inds), subjectlist, AppModelGen.Male);
AgeingModel_m = BuildAgeingModel_reg_singlelam(male_train, AppModelGen.Male, subjectlist, train.numbers(male_train_inds), min_lam_m);

min_lam_f = CrossValidation_ageEst(female_train, train.numbers(female_train_inds), subjectlist, AppModelGen.Female);
AgeingModel_f = BuildAgeingModel_reg_singlelam(female_train, AppModelGen.Female, subjectlist, train.numbers(female_train_inds), min_lam_f);

ageEstM_err = TestAgeingModel(AgeingModel_m, AppModelGen.Male, male_test, test.numbers(male_test_inds), subjectlist);
ageEstM_avgErr = mean(abs(ageEstM_err(:, 2)));

ageEstF_err = TestAgeingModel(AgeingModel_f, AppModelGen.Female, female_test, test.numbers(female_test_inds), subjectlist);
ageEstF_avgErr = mean(abs(ageEstF_err(:, 2)));

clear AppModelGen male_test female_test male_test_inds female_test_inds male_train female_train male_train_inds female_train_inds AgeingModel_m ...
    AgeingModel_f 