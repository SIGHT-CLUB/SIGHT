
function link_matrix = communication_checker(mu1_location,mu2_location,mu3_location,base_location,obstacle,n,R)

% lets say that mu1=1, mu2=2, mu3=3; base=4; scanned=5; not explored=0;
location_Array=zeros(4,2);
link_matrix=zeros(4,4);
% we find the coordinat of each mobile unit from their information matrix
[location_Array(1,1),location_Array(1,2)]=find(mu1_location==1);
[location_Array(2,1),location_Array(2,2)]=find(mu2_location==2);
[location_Array(3,1),location_Array(3,2)]=find(mu3_location==3);
[location_Array(4,1),location_Array(4,2)]=find(base_location==4);
%Grid size 
grid_size=2.5*R/n;
for i=1:3 
    for j=i+1 :4
        %calculating the difference between two units
        row_difference=location_Array(i,1)-location_Array(j,1);
        column_difference=location_Array(i,2)-location_Array(j,2);
        distance=sqrt((row_difference)^2+(column_difference)^2);
        
        real_dist=grid_size*distance;
       
        if  real_dist < R
            %This part is written to check if there is any obstacle between
            %units (obstacles are considered as walls along the tiles)
            slope=row_difference/column_difference;
            for k=1:3
            % we will use the two line equations and solve them to check if
            % there is an intersection
            % If obstacle is like a wall, it is either in form y=constant
            % or x= constant, code below determines which one it is
            obstacle_row=abs(obstacle(k,1)-obstacle(k,3));
            obstacle_column=abs(obstacle(k,2)-obstacle(k,4));
            if obstacle_row==0
            syms row col 
            %eqn1 = -slope*col + row-(location_Array(i,1)+0.5)+slope*(location_Array(i,2)+0.5) == 0;
            %eqn2 = row -obstacle(k,1) == 0;
             A=[1 -slope; 0 1];
             B=[location_Array(i,1)+0.5-slope*(location_Array(i,2)+0.5);obstacle(k,1)];
            X = linsolve(A,B);
            if (min(obstacle(k,2),obstacle(k,4)) <=X(2)) && (X(2)<=max(obstacle(k,2),obstacle(k,4)))
            link_matrix(i,j)=0;
            link_matrix(j,i)=0;
            else
            link_matrix(i,j)=1;
            link_matrix(j,i)=1;
            end
            
            elseif obstacle_column==0
            syms row col 
            %eqn1 = -slope*col + row-(location_Array(i,1)+0.5)+slope*(location_Array(i,2)+0.5) == 0;
            %eqn2 = col -obstacle(k,2) == 0;
            A=[1 -slope; 0 1];
            B=[location_Array(i,1)+0.5-slope*(location_Array(i,2)+0.5);obstacle(k,2)];
            
            X = linsolve(A,B);
            if (min(obstacle(k,1),obstacle(k,3)) <=X(1)) && (X(1)<=max(obstacle(k,1),obstacle(k,3)))
            link_matrix(i,j)=0;
            link_matrix(j,i)=0;
            else
            link_matrix(i,j)=1;
            link_matrix(j,i)=1;
            end
            end
            end 
           
            
        else 
            link_matrix(i,j)=0;
            link_matrix(j,i)=0;
        end 
    end 
end
end