function [subF,subV] = DeleteIsolateSurface(TriS, PtsS) 
%************************************************************************* 
%This function uses "TriS", face data of polygon, and "PtsS", position data 
%of polygon vertexes and detects isolated vertexes from the main mesh. 
%The outputs are "subF", new face data and "subV", new vertex positions. 
%*************************************************************************

%Detect total number of vertexes from polygon data 
[rPts, ~] = size(PtsS); 
%"n" indicates whether all vertexes are detected or not 
n = 1; 
%"nn" is the counter of loops 
nn = 1; 
%If the number of vertexes is more than half of the total vertexes, judgeN is changed to "1" 
judgeN = 0;

%Start finding isolated vertex group 
%Repeat the process until the vertex group containing more than half of the vertexes are found

while judgeN ==0  
    %Select start vertex at random  
    startnumber = round(size(PtsS(:,1))*rand);  
    startN = startnumber(1,1);  
    %if random number = 0, replace with 1  
    if startN == 0 
        startN = 1;  
    end
    %Make template vector and put start vertex number 
    template = [startN];  
    %To put rows in TriS, make a new empty vector  
    sumrowS = [];
    
    %In the following loop, all vertexes connected with startN vertex are searched
    while n == 1    
        %For vertexes specified by template vector, find row numbers in TriS   
        %containing corresponding vertexes, put the row number in rowS     
        for k = 1:size(template) 
            [rowS,~] = find(TriS == template(k)); 
            %Concatenate rowS and sumrowS and remove duplicated rows 
            sumrowS = unique([rowS;sumrowS]) ;       
        end
        
        %Make a daughter matrix "SubTriS" from TriS, which contains only rows    
        %specified by sumrowS    
        subF = TriS(sumrowS,:);   
        %"template" vector containing PtsS row numbers is transferred to   
        %another vector    
        pretemplate = template;   
        %All unique vertex numbers in subF matrix is transferred to "template"    
        template = unique(subF);
    
        %Compare sizes of "template" and "pretemplate"    
        %If they are the same, all the vertexes are judged to be detected   
        if size(template) == size(pretemplate)
            n = 0;    
        end
    end
    %Check row number of template vectors and if it is smaller than half of 
    %the total vertexes, selection of the initial "startN" was judged to be not appropriate 
    if size(template) < 0.5*rPts    
        %pretemplate is cleared    
        pretemplate = [];   
        %Count the number of trials     
        nn = nn +1; 
        %If row numbers of template vector is larger than half of the total 
        %vertexes, selection of the initial "startN" was judged to be appropriate 
        %and exit loop 
    else
        judgeN = 1; 
    end
    
    %If trial of random "startN" selection was repeated for more than 10, 
    %it is judged that there is no "majority" vertex group and exit loop 
    if nn >10    
        judgeN = 1;
    end

end

%Row numbers of selected vertexes in PtsS is stored in "template" vector 
%In the following loop with index k, row number specified by template(k) is 
%selected, elements in SubF containing this row number is replaced with %"k"
%Prepare a new Matrix for replacing elements in subF 
NewsubF = TriS(sumrowS,:);

for k = 1:size(template)    
    NewsubF(subF == template(k)) = k;    
end
%Replace subF with reordered NewsubF 
subF = NewsubF;

%To create subV, rows specified by "template" vector are extracted 
subV = PtsS(template,:);

end
    
















