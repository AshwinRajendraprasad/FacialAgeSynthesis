function DisplayAgePrototypes(age_prototypes, mask, save, save_folder)
%DisplayAgePrototypes Displays and saves the figures of the age prototypes
%   2 different formats of display, the first is a figure per age range and
%   shows all different sigma plus tau for that sigma.  Second displays the
%   best prototype (smallest tau) for each age range in a single figure
    for j=1:8
        fig = figure;
        p = age_prototypes(j);
        for i=1:size(p.sigma,2)
            subplot(4,5,i)
            imshow(AddZerosToImage(mask, p.prototype(i,:))/255)
            colormap(gray)
            axis off

            title(strcat(num2str(p.sigma(i)),':',num2str(p.tau(i))))
        end
        if save
            if j==8
                saveas(fig, strcat(save_folder, '\Age Prototype', num2str((j-1)*5+20), '+'), 'jpg')
            else
                saveas(fig, strcat(save_folder, '\Age Prototype', num2str((j-1)*5+20), '-', num2str((j-1)*5+24)), 'jpg')
            end
        end
    end

    fig =figure;
    for i=1:8
        subplot(2,4,i)
        imshow(AddZerosToImage(mask, age_prototypes(i).best_proto)/255)
        colormap(gray)
        axis off
        if i==8
            title(strcat(num2str((i-1)*5+20),'+'))
        else
            title(strcat(num2str((i-1)*5+20), '-', num2str((i-1)*5+24)))
        end
    end
    if save
        saveas(fig, strcat(save_folder, '\Age Prototype_best'), 'jpg')
    end
end