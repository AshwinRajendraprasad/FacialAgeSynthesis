function [ one_image_per_subject ] = SingleImageForEachSubject( all_images, subj_numbers, subjectlist )
%SingleImageForEachSubject Shrinks dataset so only one image for each
%subject

    already_got = zeros(size(subjectlist,1), 1);
    one_image_per_subject = zeros(size(subjectlist,1), size(all_images, 2));
    for i=1:size(all_images,1)
        subj_num = subj_numbers(i);
        if not((already_got == subj_num))
            one_image_per_subject(subj_num,:) = all_images(i,:);
            already_got(subj_num) = subj_num;
        end
    end
    % delete empty rows
    one_image_per_subject(~any(one_image_per_subject,2), :) = [];
end

