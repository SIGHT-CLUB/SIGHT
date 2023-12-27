function [followedMove, followedMoves] = movee(moves, currentPosition, mu_info)

num =randi([1 size(moves,1)]);
move = moves(num, :);
nextPosition_candidate = currentPosition + move;
followedMoves = [];

if ( all(nextPosition_candidate >= 1) && all(nextPosition_candidate <= size(mu_info)) ) && ( mu_info(nextPosition_candidate(1), nextPosition_candidate(2)) == 0 )
   followedMoves = [followedMoves; move];
elseif size(moves,1)>=2
    moves_deleted = moves;
    moves_deleted(num,:) = [];
    num =randi([1 size(moves,1)]);
   
    [~, followedMoves] = movee(moves_deleted, currentPosition, mu_info);
else
    num =randi([1 size(moves,1)]);
    move = moves(num, :);
    currentPosition = currentPosition + move;
    [~, followedMoves] = movee(moves, currentPosition, mu_info);
    followedMoves = [move; followedMoves];
end

followedMove = followedMoves(1,:);  
end