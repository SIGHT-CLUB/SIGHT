% this function is where the next movement of the unit is determined. it
% chooses the nearest empty location (if more than one such locationü random 
% one of them) as the aimed location, then constructs a trajectory to that
% point. the next move is the frst elent of the trajector.
function [trajectory_output, aimedPosition_out] = moveee(trajectory_input, currentPosition, mu_info, aimedPosition, avoidPositions)

gridSize = size(mu_info);

distanceM = zeros(size(mu_info));

for i = 1 : size(mu_info)
    for j = 1 : size(mu_info)
        distanceM(i,j) = abs(i - currentPosition(1)) + abs(j - currentPosition(2));
    end
end

distanceM = distanceM + 1000*mu_info;

minimumValue = min(distanceM, [], "all");
[index1, index2] = find(distanceM== minimumValue);
index = [index1, index2];
aimedPosition_out = index(randi([1, size(index,1)]),:);
% aimedPosition_out = index(1,:);


trajectory_output = trajectory_maker(currentPosition, aimedPosition_out);

if size(trajectory_output,1) > 1

    for i = 1:size(avoidPositions, 1)
        obstaclePosition = avoidPositions(i,:);
        trajectory_output = trajectory_fixer(trajectory_output, currentPosition, obstaclePosition, gridSize) ;
        
        if size(trajectory_output,1) > 1
            continue
         end
        %avoidObstacle(currentPosition, trajectory_output, obstaclePosition, gridSize);
        %trajectory_fixer(trajectory_output, currentPosition, obstaclePosition, gridSize) ;
    end

end


% disp("current and aimed position as a result of the algorithm")
% currentPosition
% aimedPosition_out
% disp("distance matrix without addition")
% distanceM - 1000*mu_info
% disp("info matrix")
% mu_info
% disp("distance matrix")
% distanceM
% disp("trajectory")
% trajectory_output



end