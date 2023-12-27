function robotSimulation_vol2(mu1, mu2, mu3, bu, targetLocation, numberofiteration)
    % Create a 10x10 grid
    field = zeros(10, 10);

    % Set the target location
    targetRow = targetLocation(1);
    targetCol = targetLocation(2);

    % Set the mobile unit locations
    buRow = bu(1);
    buCol = bu(2);

    mu1Row = mu1{1}(1);
    mu1Col = mu1{1}(2);

    mu2Row = mu2{1}(1);
    mu2Col = mu2{1}(2);

    mu3Row = mu3{1}(1);
    mu3Col = mu3{1}(2);

    % Set the color value for the scanned area
    highlightValue = 5;

    figure();
    colormap('white');

    imagesc(field);
    hold on;
    viscircles([targetCol, targetRow], 0.2, 'EdgeColor', 'r', 'LineWidth', 2);
    axis equal;

    yticks(0.5:10.5)
    xticks(0.5:10.5);
    xticklabels(1:10);
    yticklabels(1:10);
    axis([0.5 10.5 0.5 10.5]);
    grid on;

    plot1 = scatter(mu1Col, mu1Row, 'LineWidth', 2,'Marker','o','MarkerEdgeColor','b','MarkerFaceColor','none');
    plot2 = scatter(mu2Col, mu2Row, 'LineWidth', 2,'Marker','o','MarkerEdgeColor','y','MarkerFaceColor','none');
    plot3 = scatter(mu3Col, mu3Row, 'LineWidth', 2,'Marker','o','MarkerEdgeColor','g','MarkerFaceColor','none');
    plot4 = scatter(buCol, buRow, 'LineWidth', 6,'Marker','square','MarkerEdgeColor','k','MarkerFaceColor','none');

    % Add scatter plot of a vector of size 4
    moves_made_1= [0,0]; % Modify this vector as needed
    plotVector_1 = scatter(moves_made_1(:, 1), moves_made_1(:, 2), 'filled', 'Marker', 'x', 'MarkerEdgeColor', 'b', 'MarkerFaceColor', 'b');
    moves_made_2= [0,0]; % Modify this vector as needed
    plotVector_2 = scatter(moves_made_2(:, 1), moves_made_2(:, 2), 'filled', 'Marker', 'x', 'MarkerEdgeColor', 'y', 'MarkerFaceColor', 'y');
    moves_made_3= [0,0]; % Modify this vector as needed
    plotVector_3 = scatter(moves_made_3(:, 1), moves_made_3(:, 2), 'filled', 'Marker', 'x', 'MarkerEdgeColor', 'g', 'MarkerFaceColor', 'g');

    

    for i = 2:numberofiteration
        mu1Row = mu1{i}(1);
        mu1Col = mu1{i}(2);

        mu2Row = mu2{i}(1);
        mu2Col = mu2{i}(2);

        mu3Row = mu3{i}(1);
        mu3Col = mu3{i}(2);

        field(mu1Row, mu1Col) = highlightValue;
        field(mu2Row, mu2Col) = highlightValue;
        field(mu3Row, mu3Col) = highlightValue;

        drawnow;

        plot1.XData = mu1Col;
        plot1.YData = mu1Row;
        plot2.XData = mu2Col;
        plot2.YData = mu2Row;
        plot3.XData = mu3Col;
        plot3.YData = mu3Row;

        % Update vector scatter plot for the moves made
        
        plotVector_1.XData = moves_made_1(:, 1);
        plotVector_1.YData = moves_made_1(:, 2);

        plotVector_2.XData = moves_made_2(:, 1);
        plotVector_2.YData = moves_made_2(:, 2);

        plotVector_3.XData = moves_made_3(:, 1);
        plotVector_3.YData = moves_made_3(:, 2);


        
        pause(0.3)
        moves_made_1 = [moves_made_1; mu1Col, mu1Row];
        moves_made_2 = [moves_made_2; mu2Col, mu2Row];
        moves_made_3 = [moves_made_3; mu3Col, mu3Row];
    end

end