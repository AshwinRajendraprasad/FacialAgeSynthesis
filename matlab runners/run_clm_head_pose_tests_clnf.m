clear;

% fitting parameters more suitable for ccnf

%%
% Run the BU test with ccnf
buDir = [getenv('USERPROFILE') '/Dropbox/AAM/test data/bu/uniform-light/'];

% The fast and accurate ccnf
%%
v = 3;
[fps_bu_acc_ml, resFolderBUccnf_acc_ml] = run_bu_experiment_clm(buDir, false, false, v, 'model', 'model/main.txt');
[bu_error_ccnf_ccnf_acc_ml, ~, ~, all_errors_bu_ccnf_acc_ml] = calcBUerror(resFolderBUccnf_acc_ml, buDir);

[fps_bu_fast_ml, resFolderBUccnf_fast_ml] = run_bu_experiment_clm(buDir, false, true, v, 'model', 'model/main.txt');
[bu_error_ccnf_ccnf_fast_ml, ~, ~, all_errors_bu_ccnf_fast_ml] = calcBUerror(resFolderBUccnf_fast_ml, buDir);

%%
% Run the Biwi test
root_dir = [getenv('USERPROFILE') '/Dropbox/AAM/test data/'];
biwi_dir = '/biwi pose/';
biwi_results_root = '/biwi pose results/';

% Intensity
v = 3;
[fps_biwi_ccnf_acc, res_folder_ccnf_biwi] = run_biwi_experiment_clm(root_dir, biwi_dir, biwi_results_root, false, false, false, v, 'model', 'model/main.txt');
% Calculate the resulting errors
[biwi_error_ccnf_acc, ~, ~, ~, all_errors_biwi_ccnf_acc] = calcBiwiError([root_dir res_folder_ccnf_biwi], [root_dir biwi_dir]);

[fps_biwi_ccnf_fast, res_folder_ccnf_biwi] = run_biwi_experiment_clm(root_dir, biwi_dir, biwi_results_root, false, true, false, v, 'model', 'model/main.txt');
% Calculate the resulting errors
[biwi_error_ccnf_fast, ~, ~, ~, all_errors_biwi_ccnf_fast] = calcBiwiError([root_dir res_folder_ccnf_biwi], [root_dir biwi_dir]);

%% Run the ICT test
ict_root = [getenv('USERPROFILE') '/Dropbox/AAM/test data/'];
ict_dir = ['ict/'];
ict_results_root = ['ict results/'];

v = 3;
% Intensity
[fps_ict_ccnf_acc, res_folder_ict_ccnf] = run_ict_experiment_clm(ict_root, ict_dir, ict_results_root, false, false, false, v, 'model', 'model/main.txt');
[ict_error_ccnf_acc, ~, ~, ~, all_errors_ict_ccnf_acc] = calcIctError([ict_root res_folder_ict_ccnf], [ict_root ict_dir]);

[fps_ict_ccnf_fast, res_folder_ict_ccnf] = run_ict_experiment_clm(ict_root, ict_dir, ict_results_root, false, true, false, v, 'model', 'model/main.txt');
[ict_error_ccnf_fast, ~, ~, ~, all_errors_ict_ccnf_fast] = calcIctError([ict_root res_folder_ict_ccnf], [ict_root ict_dir]);

%% Save the results
v = 1;
filename = [sprintf('results/Pose_clm_ccnf_v%s', num2str(v))];
save(filename);