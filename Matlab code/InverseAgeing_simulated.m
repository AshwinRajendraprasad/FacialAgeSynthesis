function [ inverse_ageing, random_params ] = InverseAgeing_simulated( mdl, offset, scale, variances, modes, gbar, number_produced, mask )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    modes_stddev = sqrt(variances);
    
    random_params = zeros(number_produced, size(variances,1));
    ages = zeros(number_produced, 1);
    
    counter = 1;
    params_sofar = 0;
    while params_sofar < number_produced
        % generates normally distributed random vector with mean 0 and
        % std dev the same as the std dev of each mode of appearance,
        % giving random (but plausible) appearance parameters
        b_rand = randn(size(variances)).*modes_stddev;
        b_rand_norm = (b_rand - transpose(offset)) .* transpose(scale);
        
        % make into an image and display to check that it is a plausible
        % face
%         g_rand = gbar + transpose(modes*b_rand);
%         imshow(reshape(AddZerosToImage(mask, g_rand), size(mask))/255);
%         age = round(predict(mdl, transpose(b_rand_norm)));
%         display(age);
%         waitforbuttonpress;
        % Only use this one if the user has pressed Enter to signify that
        % it is a plausible face
%         kkey = get(gcf,'CurrentCharacter');
        if (13 == 13)
            random_params(counter, :) = b_rand;
            age = round(predict(mdl, transpose(b_rand_norm)));
            if (age > 18 && age < 60)
                ages(counter) = age;
                counter = counter+1;
                params_sofar = params_sofar+1;
            end
        end
    end
    
    inverse_ageing = zeros(max(ages), size(variances,1));
    for i=min(ages):max(ages)
        % the inverse ageing function will be a matrix where the row is the
        % age.  This isn't too much of a waste of space as the ages will
        % not go very high
        if any((ages == i))
            params = random_params(ages==i, :);
            inverse_ageing(i,:) = mean(params, 1);
        end
    end
end

