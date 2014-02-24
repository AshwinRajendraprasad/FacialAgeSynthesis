function WriteModelToCSVs(Model, path)
% Writes a full model to CSV
% The top level of the model is just 3 structures (app, ageest, agesynth)
    names = fieldnames(Model);
    filename = strcat(path, '\model.txt');
    for i=1:size(names)
        field = Model.(names{i});
        if (isstruct(field))
            path2 = strcat(path, '\', names{i});
            mkdir(path2);
            WriteModelToCSVs(field, path2);
        else
            % want the field name to name field in C++
            fid = fopen(filename, 'at+');
            fprintf(fid, '%s\n', names{i});
            fclose(fid);
            % save with 16 digits of precision (16 s.f.)
            dlmwrite(filename, field, '-append', 'newline', 'pc', 'precision', 16);
            dlmwrite(filename, ' ', '-append', 'newline', 'pc');
        end
    end
end