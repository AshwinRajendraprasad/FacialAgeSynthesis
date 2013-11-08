oldDir = chdir('../Release/');
clm_exe = '"SimpleCLM.exe"';

output = '../matlab runners/demo_vid/';

if(~exist(output, 'file'))
    mkdir(output)
end
    
in_files = dir('../videos/*.avi');

% some parameters
fast = true;
verbose = true;

%model = 'model/main.txt'; % slow to load, but reasonably fast when it gets going (also you can specify multiple videos at once, see the head pose tests)
model = 'model/main_svr_ml.txt';% (faster but less accurate)
%model = 'model/main_svr.txt';% (faster but less accurate and single light)
%model = 'model/main_wild.txt';% (trained on in the wild data, not as accurate in clear environments but much more robust)

tic;
for i=1:numel(in_files)
    
    command = clm_exe;
        
    inputFile = ['../videos/', in_files(i).name];
    [~, name, ~] = fileparts(inputFile);
    
    % where to output results
    outputFile = [output name '.txt'];
    
    command = cat(2, command, [' -f "' inputFile '" -op "' outputFile '"']);
    
    if(verbose)
        outputVideo = ['"' output name '.avi' '"'];
        command = cat(2, command, [' -ov ' outputVideo]);
    end
        
    if(fast)
        command = cat(2, command, ' -clmfast ');
    else
        command = cat(2, command, ' -clmacc ');
    end
    
    command = cat(2, command,  ' -fx 500 -fy 500 -cx 160 -cy 120');
    
    command = cat(2, command, [' -mloc "', model, '"']);
    
    dos(command);
end

chdir(oldDir);