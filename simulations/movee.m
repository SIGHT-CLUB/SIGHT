function [followedMove, followedMoves] = movee(moves, currentPosition, mu_info)

moves_deleted = moves;
followedMoves = [];

while(size(moves_deleted,1)>=1)
num =randi([1 size(moves_deleted,1)]);
move = moves_deleted(num, :);
nextPosition_candidate = currentPosition + move;
if ( all(nextPosition_candidate >= 1) && all(nextPosition_candidate <= size(mu_info)) ) && ( mu_info(nextPosition_candidate(1), nextPosition_candidate(2)) == 0 )
    followedMoves = move;
    break
end
moves_deleted(num,:) = [];
end

if isempty(followedMoves)
    
    while(1)
        num =randi([1 size(moves,1)]);
        move = moves(num, :);
        nextPosition_candidate = currentPosition + move;
        if ( all(nextPosition_candidate >= 1) && all(nextPosition_candidate <= size(mu_info)) )
            break
        end
    end    
    
    currentPosition = currentPosition + move;
    [~, followedMoves] = movee(moves, currentPosition, mu_info);
    followedMoves = [move; followedMoves];
end

followedMove = followedMoves(1,:);
end