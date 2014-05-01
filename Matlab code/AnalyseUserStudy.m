%% Analyse ageing results

errors_age(3).err = zeros(30,1);
errors_age(3).actual_err = zeros(30,10);

for type=1:3
    
    for index=1:30
        guess = zeros(10,1);
        
        offset = user_study_info(index).perm(type);
        if type==1
            actual = user_study_info(index).age;
        else
            actual = user_study_info(index).target_ages(type-1);
        end
        
        image = file_perm(index);
        for subj=1:10
            guess(subj) = UserStudyAgeResults(image, (subj-1)*3+offset);
            errors_age(type).actual_err(index,subj) = guess(subj) - actual;
        end
        errors_age(type).abserr(index) = mean(abs(guess-actual));
        errors_age(type).err(index) = mean(guess-actual);
    end
    errors_age(type).avg = mean(errors_age(type).err);
    errors_age(type).absavg = mean(errors_age(type).abserr);
end

clear guess offset actual image type index subj; 

%% Analyse similarity results
similarity(2).sim = zeros(30,1);
for type=2:3
    for index=1:30
        sim = zeros(10,1);
        orig = user_study_info(index).perm(1);
        compare = user_study_info(index).perm(type);
        if (orig==1 && compare==2) || (compare==1 && orig==2)
            offset = 1;
        elseif (orig==2 && compare==3) || (compare==2 && orig==3)
            offset = 2;
        elseif (orig==1 && compare==3) || (compare==1 && orig==3)
            offset = 3;
        end
        
        image = file_perm(index);
        for subj=1:10
            sim(subj) = UserStudySimResults(image, (subj-1)*3+offset);
        end
        similarity(type-1).sim(index) = mean(sim);
    end
    similarity(type-1).avg = mean(similarity(type-1).sim);
end

clear type index orig offset subj image sim compare;
        