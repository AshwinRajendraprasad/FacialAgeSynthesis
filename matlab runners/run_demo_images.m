clear

oldDir = chdir('../Release/');
clm_exe = '"SimpleCLMImg.exe"';

output = '../matlab runners/demo_vid/';

if(~exist(output, 'file'))
    mkdir(output)
end
    
in_dir  = '../videos/';
out_dir = '../matlab runners/demo_img/';

if(~exist(out_dir, 'file'))
    mkdir(out_dir);
end

% some parameters
fast = true;
verbose = true;

%model = 'model/main.txt'; % slow to load, but reasonably fast when it gets going
%model = 'model/main_svr_ml.txt'; %(faster but less accurate)
%model = 'model/main_svr.txt'; %(faster but less accurate and single light)
model = 'model/main_wild.txt'; %(trained on in the wild data, not as accurate in clear environments but much more robust)

command = clm_exe;

command = cat(2, command, [' -fdir "' in_dir '"']);

if(verbose)
    command = cat(2, command, [' -ofdir "' out_dir '"']);
    command = cat(2, command, [' -oidir "' out_dir '"']);
end

if(fast)
    command = cat(2, command, ' -clmfast ');
else
    command = cat(2, command, ' -clmacc ');
end

command = cat(2, command, [' -mloc "', model, '"']);

dos(command);

chdir(oldDir);