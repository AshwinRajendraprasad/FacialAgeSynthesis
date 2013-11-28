warpedSize = size(allWarped);
gbar = zeros(warpedSize(2),warpedSize(3));
for i=1:warpedSize(1)
    gbar = gbar + squeeze(allWarped(i,:,:));
end
gbar = gbar / warpedSize(1);