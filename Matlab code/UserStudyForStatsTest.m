%% Get ace accuracy data for entry to SPSS

errors_age_stats(3).err = zeros(10,1);

for type=1:3
    for subj=1:10
        errors = zeros(30,1);
    
        for index=1:30

            offset = user_study_info(index).perm(type);
            if type==1
                actual = user_study_info(index).age;
            else
                actual = user_study_info(index).target_ages(type-1);
            end

            image = file_perm(index);
            errors(index) = UserStudyAgeResults(image, (subj-1)*3+offset)-actual;
        end
        errors_age_stats(type).err(subj) = mean(abs(errors));
    end
    errors_age_stats(type).avg = mean(errors_age_stats(type).err);
end

ageStats(:,1) = errors_age_stats(1).err;
ageStats(:,2) = errors_age_stats(2).err;
ageStats(:,3) = errors_age_stats(3).err;

clear errors offset actual image type index subj;

%% Get similarity data for entry to SPSS

similarity_stats(2).sim = zeros(10,1);
for type=2:3
    for subj=1:10
        sim = zeros(30,1);
        for index=1:30
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
            sim(index) = UserStudySimResults(image, (subj-1)*3+offset);
        end
        similarity_stats(type-1).sim(subj) = mean(sim);
    end
    similarity_stats(type-1).avg = mean(similarity_stats(type-1).sim);
end

simStats(:,1) = similarity_stats(1).sim;
simStats(:,2) = similarity_stats(2).sim;
simStats(:,3) = simStats(:,1) - simStats(:,2);

clear type index orig offset subj image sim compare;