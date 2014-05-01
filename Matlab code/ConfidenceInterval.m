function confidence = ConfidenceInterval(data)

    sx = std(data);
    confidence = 1.96*sx/sqrt(size(data,1));
    
end