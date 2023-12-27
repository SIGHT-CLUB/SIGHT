choose one from possible moves
next = current + move
if next empty and valid
    enter
    moves = [moves; move]
else if next not empty or valid
    delete from the possible moves
    try again till no possible moves are remaining
else if still no moves
        choose one from original moves, enter
        moves = [move]
        do the same
end

moves = moves[1,:]