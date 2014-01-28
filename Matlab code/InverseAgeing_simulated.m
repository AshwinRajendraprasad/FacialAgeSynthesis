function [ inverse_ageing, random_params, ages_quant ] = InverseAgeing_simulated( AgeingModel, AppearanceModel, maxNumber, mask )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    
    AGE_MIN = 20;
    AGE_MAX = 49;
    QUANT = 2000;
    NUM_RANGE = 6;
    modes_stddev = sqrt(AppearanceModel.variances);
    
    random_params = zeros(QUANT*(AGE_MAX-AGE_MIN+1), size(AppearanceModel.variances,1));
    ages = zeros(QUANT*(AGE_MAX-AGE_MIN+1), 1);
    ages_quant = zeros(NUM_RANGE, 1);
    
    counter = 1;

    for i=1:maxNumber
        % generates normally distributed random vector with mean 0 and
        % std dev the same as the std dev of each mode of appearance,
        % giving random (but plausible) appearance parameters
        b_rand = randn(size(AppearanceModel.variances)).*modes_stddev;
        
        % clamp the random parameters so they aren't larger than 3 std
        % dev
%         larger = b_rand > 3*modes_stddev;
%         if any(larger)
%             b_rand(larger) = 3*modes_stddev(larger);
%         end
%         smaller = -b_rand > 3*modes_stddev;
%         if any(smaller)
%             b_rand(smaller) =-3*modes_stddev(smaller);
%         end
        
        % make into an image and display to check that it is a plausible
        % face
%         g_rand = AppearanceParams2Texture(b_rand, AppearanceModel);
%         imshow(AddZerosToImage(mask, g_rand)/255);
%         age = round(PredictAge(AgeingModel, b_rand));
%         display(age);
%         waitforbuttonpress;
%         Only use this one if the user has pressed Enter to signify that
%         it is a plausible face
%         kkey = get(gcf,'CurrentCharacter');
%         if (kkey == 13)
        if true
            age = round(PredictAge(AgeingModel, b_rand));
            age_range = ceil((age-19)/5);
            if (age > (AGE_MIN-1) && age < (AGE_MAX+1) && ages_quant(age_range)<QUANT)
                ages(counter) = age;
                ages_quant(age_range) = ages_quant(age_range)+1;
                random_params(counter, :) = b_rand;
                counter = counter+1;
            end
        end
        
        if ages_quant()==QUANT
            break
        end
    end
    
    % Remove the zero valued rows
    ages(~any(ages,2)) = [];
    random_params(~any(random_params,2),:) = [];
    
    inverse_ageing = zeros(NUM_RANGE, size(AppearanceModel.variances,1));
    for i=1:NUM_RANGE
        % the inverse ageing function will be a matrix where the row is the
        % age.  This isn't too much of a waste of space as the ages will
        % not go very high
        params = random_params((ages>((i-1)*5+19))&(ages<(i*5+19)), :);
        inverse_ageing(i,:) = mean(params, 1);
    end
end

