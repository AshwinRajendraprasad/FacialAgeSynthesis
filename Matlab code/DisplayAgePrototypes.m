for j=1:8
    fig = figure;
    p = age_prototypes(j);
    for i=1:19
        subplot(4,5,i)
        imagesc(AddZerosToImage(mask, p.prototype(i,:)))
        colormap(gray)
        axis off
        
        title(strcat(num2str(0.1 + (i-1)*0.05),':',num2str(p.tau(i))))
    end
    if j==8
        saveas(fig, strcat('C:\Users\Rowan\Pictures\Project\Age Prototype', num2str((j-1)*5+20), '+'), 'jpg')
    else
        saveas(fig, strcat('C:\Users\Rowan\Pictures\Project\Age Prototype', num2str((j-1)*5+20), '-', num2str((j-1)*5+24)), 'jpg')
    end
end

fig =figure;
for i=1:8
    subplot(2,4,i)
    imagesc(AddZerosToImage(mask, age_prototypes(i).best_proto))
    colormap(gray)
    axis off
    if i==8
        title(strcat(num2str((i-1)*5+20),'+'))
    else
        title(strcat(num2str((i-1)*5+20), '-', num2str((i-1)*5+24)))
    end
end
saveas(fig, 'C:\Users\Rowan\Pictures\Project\Age Prototype_best', 'jpg')