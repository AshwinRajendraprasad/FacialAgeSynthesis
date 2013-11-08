clear;

%%
% Run the BU test with CLM
buDir = [getenv('USERPROFILE') '/Dropbox/AAM/test data/bu/uniform-light/'];

% The fast and accurate single light models
%%
v = 1;
[fps_bu_acc_ml, resFolderBUCLM_acc_ml] = run_bu_experiment_clm(buDir, false, false, v, 'model', 'model/main_svr_ml.txt');
[bu_error_clm_svr_acc_ml, ~, ~, all_errors_bu_svr_acc_ml] = calcBUerror(resFolderBUCLM_acc_ml, buDir);

[fps_bu_fast_ml, resFolderBUCLM_fast_ml] = run_bu_experiment_clm(buDir, false, true, v, 'model', 'model/main_svr_ml.txt');
[bu_error_clm_svr_fast_ml, ~, ~, all_errors_bu_svr_fast_ml] = calcBUerror(resFolderBUCLM_fast_ml, buDir);

%%
% The fast and accurate multi light models
v = 2;
[fps_bu_acc_sl, resFolderBUCLM_acc_sl] = run_bu_experiment_clm(buDir, false, false, v, 'model', 'model/main_svr.txt');
[bu_error_clm_svr_acc_sl, ~, ~, all_errors_bu_svr_acc_sl] = calcBUerror(resFolderBUCLM_acc_sl, buDir);

[fps_bu_fast_sl, resFolderBUCLM_fast_sl] = run_bu_experiment_clm(buDir, false, true, v, 'model', 'model/main_svr.txt');
[bu_error_clm_svr_fast_sl, ~, ~, all_errors_bu_svr_fast_sl] = calcBUerror(resFolderBUCLM_fast_sl, buDir);

%%
% Run the Biwi test
root_dir = [getenv('USERPROFILE') '/Dropbox/AAM/test data/'];
biwi_dir = '/biwi pose/';
biwi_results_root = '/biwi pose results/';

% Intensity
v = 1;
[fps_biwi_clm_acc, res_folder_clm_biwi] = run_biwi_experiment_clm(root_dir, biwi_dir, biwi_results_root, false, false, false, v, 'model', 'model/main_svr_ml.txt');
% Calculate the resulting errors
[biwi_error_clm_acc, ~, ~, ~, all_errors_biwi_clm_acc] = calcBiwiError([root_dir res_folder_clm_biwi], [root_dir biwi_dir]);

[fps_biwi_clm_fast, res_folder_clm_biwi] = run_biwi_experiment_clm(root_dir, biwi_dir, biwi_results_root, false, true, false, v, 'model', 'model/main_svr_ml.txt');
% Calculate the resulting errors
[biwi_error_clm_fast, ~, ~, ~, all_errors_biwi_clm_fast] = calcBiwiError([root_dir res_folder_clm_biwi], [root_dir biwi_dir]);

% Intensity with depth
v = 2;
[fps_biwi_clmz_acc, res_folder_clmz_biwi] = run_biwi_experiment_clm(root_dir, biwi_dir, biwi_results_root, false, false, true, v, 'model', 'model/main_svr_ml.txt');
% Calculate the resulting errors
[biwi_error_clmz_acc, ~, ~, ~, all_errors_biwi_clm_z_acc] = calcBiwiError([root_dir res_folder_clmz_biwi], [root_dir biwi_dir]);

[fps_biwi_clmz_fast, res_folder_clmz_biwi_fast] = run_biwi_experiment_clm(root_dir, biwi_dir, biwi_results_root, false, true, true, v, 'model', 'model/main_svr_ml.txt');
% Calculate the resulting errors
[biwi_error_clmz_fast, ~, ~, ~, all_errors_biwi_clm_z_fast] = calcBiwiError([root_dir res_folder_clmz_biwi_fast], [root_dir biwi_dir]);

%% Run the ICT test
ict_root = [getenv('USERPROFILE') '/Dropbox/AAM/test data/'];
ict_dir = ['ict/'];
ict_results_root = ['ict results/'];

v = 1;
% Intensity
[fps_ict_clm_acc, res_folder_ict_clm] = run_ict_experiment_clm(ict_root, ict_dir, ict_results_root, false, false, false, v, 'model', 'model/main_svr_ml.txt');
[ict_error_clm_acc, ~, ~, ~, all_errors_ict_clm_acc] = calcIctError([ict_root res_folder_ict_clm], [ict_root ict_dir]);

[fps_ict_clm_fast, res_folder_ict_clm] = run_ict_experiment_clm(ict_root, ict_dir, ict_results_root, false, true, false, v, 'model', 'model/main_svr_ml.txt');
[ict_error_clm_fast, ~, ~, ~, all_errors_ict_clm_fast] = calcIctError([ict_root res_folder_ict_clm], [ict_root ict_dir]);

v = 2;
% Intensity and depth
[fps_ict_clmz_acc, res_folder_ict_clmz] = run_ict_experiment_clm(ict_root, ict_dir, ict_results_root, false, false, true, v, '', true, false, false, 'model', 'model/main_svr_ml.txt');
[ict_error_clmz_acc, ~, ~, ~, all_errors_ict_clm_z_acc] = calcIctError([ict_root res_folder_ict_clmz], [ict_root ict_dir]);

[fps_ict_clmz_fast, res_folder_ict_clmz] = run_ict_experiment_clm(ict_root, ict_dir, ict_results_root, false, true, true, v, '', true, true, false, 'model', 'model/main_svr_ml.txt');
[ict_error_clmz_fast, ~, ~, ~, all_errors_ict_clm_z_fast] = calcIctError([ict_root res_folder_ict_clmz], [ict_root ict_dir]);

%% Save the results
v = 1;
filename = [sprintf('results/Pose_clm_svr_v%s', num2str(v))];
save(filename);