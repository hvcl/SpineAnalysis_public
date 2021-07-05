function [Istck2, MaxN2] = RefineSpineLocation(Istck, MaxN) 
%************************************************************************* 
%This function use "Istck", TIF binary stack file and "MaxN", the number of 
%spine candidates as inputs. %Judge spine candidates whether they should be accepted or rejected 
%from their size, shape, and location. 
%The outputs of the function are "Istck2", the image stack from which 
%non-spine voxel clusters are removed and "MaxN2", which provides the new 
%number of spine candidates. 
%*************************************************************************

%************************************************************************* 
%START Remove objects too small, too elongated, or located at image edges 
%************************************************************************* 

%Check size of 3D image matrix 
Nsize = size(Istck); 
%Create new 3D image matrix 
Istck2 = zeros(Nsize(1), Nsize(2), Nsize(3)); 
%Prepare vectors for storing parameters 
OrderNo = zeros(MaxN,2); 
CheckMsize = zeros(MaxN, 1); 
CheckM2size = zeros(MaxN, 1);
CheckMRatio = zeros(MaxN, 1);
MaxN2 = 0;

%START evaluation of individual spine candidates 
for k = 1:MaxN    
    %Isolate kth spine candidate    
    CheckM1 = Istck==k;    
    %Expand matrix in Z direction 4 times    
    %CheckM = repelem(CheckM1, 1, 1, 4);    
    CheckM=CheckM1;
    %Select spine candidates larger than 400 voxel£¨corresponding to 0.01 cubic micron)    
    %and not at the edge of image field     
    if sum(sum(sum(CheckM,3),2),1) > 400 && sum(sum(sum(CheckM(:,1:3,:)))) == 0 && sum(sum(sum(CheckM (:,Nsize(2)-2:Nsize(2), :)))) == 0 
        %START evaluation if the object is elongated along X axis 
        %Find maximum and minimum X values of object 
        [~,MXrange] = find(sum((sum(CheckM,3)),1)); 
        %Calculate the total voxel size of spine candidate 
        CheckMsize(k) = sum(sum(sum(CheckM,3),2),1); 
        %Calculate the largest cross-section in YZ plane of spine candidate 
        CheckM2size(k) = max(sum(sum(CheckM,3),1)); 
        %Calculate the ratio of cross-section against X axis length
        CheckMRatio(k) = CheckM2size(k)/(max(MXrange)-min(MXrange));
        %If the ratio is larger than 2 or 4, dependent on spine size,
        %it is judged not to be elongated 
        if (CheckMRatio(k) > 2 && CheckMsize(k) < 12000) || (CheckMRatio(k) > 4 && CheckMsize(k) >= 12000) 
            %Count the number of spine candidates that passed the test 
            MaxN2 = MaxN2 +1; 
            %To the new 3D matrix, input voxel value of MaxN2 
            Istck2 = Istck2 + CheckM1*MaxN2;   
            %Integrate voxel values to X axis to estimate the spine position 
            Order = sum(sum(CheckM1, 3),1); 
            %Obtain X position of the most lefthand voxel of the spine 
            OrderNo(MaxN2,:) = [find(Order,1), MaxN2]; 
        end
    end
end
%OrderNo vector contains zero values, corresponding to candidates judged to be not spines
%These elements are identified 
IndOrderNo = OrderNo(:,1)>0; 
%From OrderNo vector, remove rows with zero elements 
OrderNo = OrderNo(IndOrderNo,:); 
%Resort rows of OrderNo from smaller to larger values 
%For this purpose, first resort OrderNo along the X positions in the first column 
ReOrderNo = sortrows(OrderNo); 
for k = 1:MaxN2    
    %ReOrderNo vector contains the spine order from the left in the first column    
    %To 3D image stack Istck2, input values of k*1000 to the spines kth from the left    
    %This multiplication helps segregation of old and new orders    
    Istck2(Istck2 == ReOrderNo(k,2)) = k*1000; 
end

%Pixel value was shifted to 1000-fold larger value 
%This should be returned to the original range 
Istck2 = Istck2/1000;

%************************************************************************* 
%END Remove objects too small, too elongated, or located at image edges
%*************************************************************************




