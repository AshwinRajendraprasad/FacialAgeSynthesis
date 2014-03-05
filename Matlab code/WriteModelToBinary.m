function WriteModelToBinary(Model, path)
% Writes a full model to binary file
% The top level of the model is just 3 structures (app, ageest, agesynth)
    names = fieldnames(Model);
    filename = strcat(path, '\model.bin');
    for i=1:size(names)
        field = Model.(names{i});
        if (isstruct(field))
            path2 = strcat(path, '\', names{i});
            mkdir(path2);
            WriteModelToBinary(field, path2);
        else
            % want the field name to name field in C++
            fid = fopen(filename, 'a');
            % write the name, a zero char, the dimension of the field (as 2
            % ints) then the data
            fwrite(fid, names{i});
            fwrite(fid, 0, 'char');
            fwrite(fid, size(field,1), 'int');
            fwrite(fid, size(field,2), 'int');
            % save the field in binary format
            fwrite(fid, field, 'double');
            fclose(fid);
        end
    end
end