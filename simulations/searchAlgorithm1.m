% this function will take a location matrix and move to empty locations where the value of empty locations are 0
%  The values in the info matrix represent as follows;
%  mu1 is here ==1 
%  mu2 is here ==2
%  mu3 is here ==3
%  mu4 is here ==4
%  This tile is scanned before is ==5
%  This tile has not been scanned ==0
%  The target is there ==6

function [mu_all,mu_info]=searchAlgorithm1(mu,mu_info,targetPosition)

    % mu: is a number between [1,3] represents the mu number
    % mu_info:10x10 matrix representing the information known by mu
   

    % Obtain the row and column of mu
    [currentRow,currentCol] =find(mu_info==mu);
    currentPosition=[currentRow,currentCol];
    targetRow=targetPosition(1);
    targetCol=targetPosition(2);
 
    numberofiterations=55;
    
    %Create empty arrays for recording the mu's locations for all time
    mu_all=cell(1,numberofiterations);
    %set initial locations for the 1st element
    mu_all{1}=[currentRow,currentCol];


    % Define possible moves (up, down, right, left)
    moves = [0, 1; 0, -1; 1, 0; -1, 0];
    
    for i=2:numberofiterations
        num =randi([1 4]); % random number decide which way to move for mu1
        nextPosition = currentPosition + moves(num, :);

        %Collect the data of mu past location
        mu_all{i}=[currentPosition(1), currentPosition(2)];

        if([currentPosition(1), currentPosition(2)] == [targetRow,targetCol])
            continue %wait for help
        % Check if the next position is within the grid
        elseif all(nextPosition >= 1) && all(nextPosition <= size(mu_info))

            %Check if the next position is target
            if [nextPosition(1), nextPosition(2)] == [targetRow,targetCol]

                %Change the value of currentPosition as targetishere
                mu_info(nextPosition(1), nextPosition(2))=6;

                %Change the value of currentPosition as scanned
                mu_info(currentPosition(1), currentPosition(2))=5;

                % Move to the target location and stay there
                currentPosition=nextPosition;
                            

            % Check if the next position is empty (field value is 0)
            elseif mu_info(nextPosition(1), nextPosition(2)) == 0
                %Change the value of currentPosition as scanned
                mu_info(currentPosition(1), currentPosition(2))=5;
                % Move to the empty location
                currentPosition = nextPosition;
                %give the information of the location of where the mu is.
                mu_info(nextPosition(1), nextPosition(2))=mu;
          
            
            
                
            end
        % elseif % enters if no empty neighbor is found.
            
            
            
        end
    end
% If no empty neighbor is found, stay in the current position
   
end















  
                
    