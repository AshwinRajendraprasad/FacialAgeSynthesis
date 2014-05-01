avg_error = zeros(580,1);

for i=1:580
    b = FindModelParameters(Model.App, textures_new(i,:));
    texture = AppearanceParams2Texture(b, Model.App);
        
    avg_error(i) = rms(texture' - textures_new(i,:));
end

clear b texture