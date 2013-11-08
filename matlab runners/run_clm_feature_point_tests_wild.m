clear

CLM_dir = [getenv('USERPROFILE') '/Dropbox/AAM/'];

addpath([CLM_dir, '/PDMhelpers']);
              
curr_dir = cd('.');

%% Run using CLNF in the wild model
out_clnf = [curr_dir '/out_wild_clnf_wild/'];
if(~exist(out_clnf, 'file'))
   mkdir(out_clnf); 
end

[err_clnf_wild, err_no_out_clnf_wild] = Run_CLM_fitting_on_images(out_clnf, 'use_afw', 'use_lfpw', 'use_ibug', 'use_helen', 'verbose', 'model', 'model/main_wild.txt', 'multi_view', 1);

%% Run using SVR model
out_svr = [curr_dir '/out_wild_svr_wild/'];
if(~exist(out_svr, 'file'))
   mkdir(out_svr); 
end

[err_svr_wild, err_no_out_svr_wild] = Run_CLM_fitting_on_images(out_svr, 'use_afw', 'use_lfpw', 'use_ibug', 'use_helen', 'verbose', 'model', 'model/main_svr_wild.txt', 'multi_view', 1);                

%% Run using general CLNF model
out_clnf = [curr_dir '/out_wild_clnf/'];
if(~exist(out_clnf, 'file'))
   mkdir(out_clnf); 
end

[err_clnf, err_no_out_clnf] = Run_CLM_fitting_on_images(out_clnf, 'use_afw', 'use_lfpw', 'use_ibug', 'use_helen', 'verbose', 'model', 'model/main.txt', 'multi_view', 1);
                      
%% Run using SVR model
out_svr = [curr_dir '/out_wild_svr/'];
if(~exist(out_svr, 'file'))
   mkdir(out_svr); 
end

[err_svr, err_no_out_svr] = Run_CLM_fitting_on_images(out_svr, 'use_afw', 'use_lfpw', 'use_ibug', 'use_helen', 'verbose', 'model', 'model/main_svr_ml.txt', 'multi_view', 1);                

%%
save('results/landmark_detections.mat');