function [VNo, NewN, NewF, NewV]= SpineMeshCalc(Istck, Istck2, k,doublingFactor)

%************************************************************************* 
%This function use "Istck", TIF binary stack file containing spine candidate 
%voxels, "Istck2", TIF binary stack file containing spine/shaft junction 
%voxels, and "k", specify the spine to be analyzed, and generate polygon
%mesh for kth spine. 
%The outputs of the function are "VNo", the number of vertexes before 
%removal of junction, "NewN", the number of vertexes after removal of 
%junction, "NewF", polygon data for faces, "NewV", polygon data for 
%vertexes positions %This function requires two sub-functions, "MarchingCubes" and 
%"DeleteIsolateSurface" 
%*************************************************************************

%Obtain size information of 3D image matrix    
%Nsize = size(Istck); 
%Select the kth spine and get voxel information
t=Istck == k;
if doublingFactor~=1
    t=imresize3(double(t),doublingFactor,'linear') > 0.1;
end
Nsize = size(t);
spineindex = find(t);

%From spineindex, restore x, y, z values    
Zsize = floor((spineindex-1)/(Nsize(1)*Nsize(2)))+1;    
Residual = rem((spineindex-1),(Nsize(1)*Nsize(2)));    
Xsize = floor(Residual/Nsize(1))+1;    
Ysize = rem(Residual,Nsize(1))+1; 
%From the restored positions, obtain max and min positions    
Xmax = max(Xsize);    
Xmin = min(Xsize);   
Ymax = max(Ysize);    
Ymin = min(Ysize);    
Zmax = max(Zsize);    
Zmin = min(Zsize); 
%For the kth spine, generate 3D matrix as a container    
Cpre = zeros(Ymax-Ymin+3, Xmax-Xmin+3, Zmax-Zmin+3); 
%From the 3D image matrix, isolate voxels for kth spine    

Istck12k = (t)*k; 
%To the container, spine voxel values are loaded    
Cpre(2:(Ymax-Ymin+2), 2:(Xmax-Xmin+2), 2:(Zmax-Zmin+2)) = Istck12k(Ymin:Ymax, Xmin:Xmax, Zmin: Zmax);
%As Z direction is one fourth of resolution, expand 3D matrix in this direction   
C=Cpre;
%C=imresize3(C,doublingFactor);
%To apply MarchingCubes function, new meshgrid is generated    
[X,Y,Z] = meshgrid((Xmin-1:Xmin-1+size(C,2)-1)*0.032/doublingFactor,(Ymin-1:Ymin-1+size(C,1)-1)*0.032/doublingFactor,(Zmin-1:Zmin-1+size(C,3)-1)*0.032/doublingFactor); 
%Calculate polygon data using MarchingCubes function    
[F,V] = MarchingCubes(X,Y,Z,C,0.5);


%Identify kth spine-shaft junction and obtain position information    
t=Istck2 == k;
if doublingFactor~=1
    t=imresize3(double(t),doublingFactor,'linear') > 0.1;
end
Nsize = size(t);
spineindexB = find(t); 
%Restore x, y, z values    
ZsizeB = floor((spineindexB-1)/(Nsize(1)*Nsize(2)))+1;    
ResidualB = rem((spineindexB-1),(Nsize(1)*Nsize(2)));    
XsizeB = floor(ResidualB/Nsize(1))+1;    
YsizeB = rem(ResidualB,Nsize(1))+1; 
%Convert x, y, z position information into absolute values   
XYZabsSize = [XsizeB*0.032/doublingFactor, YsizeB*0.032/doublingFactor, ZsizeB*0.032/doublingFactor]; 
%Count row numbers in V and XYZabsSize    
[VNo, ~] = size(V);    
[BNo, ~] = size(XYZabsSize);

%Prepare new matrix for vertexes    
NewV = V; 
%Prepare new matrix for faces    
NewF = F; 
%Prepare vector to record reordering of vertexes    
Template = zeros(VNo,1);
%Prepare a variable to record vertex number to be saved    
NewN = 0;

%For all vertexes in spine polygon, calculate distance from each voxels within junction
for kk = 1:VNo 
    %Subtract two vectors (spine vertex and junction) 
    Dist = repmat(V(kk,:),BNo,1)-XYZabsSize; 
    %Calculate distance 
    CalDist = sqrt(Dist(:,1).^2 + Dist(:,2).^2 + Dist(:,3).^2);
    %If minimal distance is less than 120 nm, the vertex is marked by 
    %putting zero 
    if min(CalDist) < 0.035/doublingFactor 
        NewV(kk,:) = [0 0 0]; 
        NewF(NewF == kk) = 0; 
    else
        NewN = NewN+1;
        %Put current vertex number in Template vector 
        Template(NewN) = kk;   
    end
end
%Template vector contains excess "zero" elements at the end    
%Delete "zero" from Template    
Template = Template(1:NewN,:);
%To delete close-to-junction vertex, generate vector containing zero at    
%any of columns in NewF matrix    
NewFVect = NewF(:,1).*NewF(:,2).*NewF(:,3);    
%From the above vector, obtain non-zero row numbers     
IndNewFVect = NewFVect>0;    
%From NewF matrix, remove rows that contains zero   
NewF = NewF(IndNewFVect, :);   
%Reorder vertex numbers in NewF matrix according to the orders   
%specified in Template vector    
for m = 1:NewN 
    NewF(NewF == Template(m,1))= m;    
end

%From NewV matrix, obtain row number data that are non-zero    
IndNewVVect = NewV(:,1)>0;    
%From NewV matrix, delete rows containing zero   
NewV = NewV(IndNewVVect, :);


%% mesh refinment for step artifact removing
% tV=NewV*doublingFactor/0.032*2;
% tV(:,1)=round(tV(:,1)-min(tV(:,1))+1);
% tV(:,2)=round(tV(:,2)-min(tV(:,2))+1);
% tV(:,3)=round(tV(:,3)-min(tV(:,3))+1);
%  
% 
% vIndex=zeros(max(tV(:,1)),max(tV(:,2)),max(tV(:,3)));
% 
% Nsize=size(vIndex);
% 
% 
% for i=1:size(NewV,1)
%     vIndex(tV(i,1),tV(i,2),tV(i,3))=i;
% end
% 
% connectMatrix=zeros(size(NewV,1),size(NewV,1));
% for i=1:size(NewF,1)
%     connectMatrix(NewF(i,1),NewF(i,2))=1;
%     connectMatrix(NewF(i,2),NewF(i,1))=1;
%     connectMatrix(NewF(i,2),NewF(i,3))=1;
%     connectMatrix(NewF(i,3),NewF(i,2))=1;
%     connectMatrix(NewF(i,3),NewF(i,1))=1;
%     connectMatrix(NewF(i,1),NewF(i,3))=1;
% end
% 
% for x=1:Nsize(1)
%     for y=1:Nsize(2)
%         startV=1;
%         prev=1;
%         cnt=0;
%         newPos=zeros(1,3);
%         for z=1:Nsize(3)
%             if vIndex(x,y,z)==0
%                 continue;
%             end
%             if cnt==0
%                 startV=vIndex(x,y,z);
%                 prev=z;
%                 cnt=1;
%                 newPos=NewV(vIndex(x,y,z),:);
%                 continue;
%             end
%             if connectMatrix(vIndex(x,y,prev),vIndex(x,y,z))==1
%                 cnt=cnt+1;
%                 newPos=newPos+NewV(vIndex(x,y,z),:);
%                 NewF(NewF==vIndex(x,y,z))=startV;
%             else
%                 NewV(startV,:)=newPos/cnt;
%                 cnt=0;
%                 newPos=zeros(1,3);
%             end
%             prev=z;
%         end
%         if cnt~=0
%                 NewV(startV,:)=newPos/cnt;
%         end
%             
%     end
% end
% 
% NewF(NewF(:,1)==NewF(:,2)|NewF(:,1)==NewF(:,3)|NewF(:,2)==NewF(:,3),:)=0;
% NewF=NewF(NewF(:,1)~=0,:);
% 
% tV=NewV*doublingFactor/0.032*2;
% tV(:,1)=round(tV(:,1)-min(tV(:,1))+1);
% tV(:,2)=round(tV(:,2)-min(tV(:,2))+1);
% tV(:,3)=round(tV(:,3)-min(tV(:,3))+1);
%  
% 
% vIndex=zeros(max(tV(:,1)),max(tV(:,2)),max(tV(:,3)));
% 
% Nsize=size(vIndex);
% 
% 
% for i=1:size(NewV,1)
%     vIndex(tV(i,1),tV(i,2),tV(i,3))=i;
% end
% 
% connectMatrix=zeros(size(NewV,1),size(NewV,1));
% for i=1:size(NewF,1)
%     connectMatrix(NewF(i,1),NewF(i,2))=1;
%     connectMatrix(NewF(i,2),NewF(i,1))=1;
%     connectMatrix(NewF(i,2),NewF(i,3))=1;
%     connectMatrix(NewF(i,3),NewF(i,2))=1;
%     connectMatrix(NewF(i,3),NewF(i,1))=1;
%     connectMatrix(NewF(i,1),NewF(i,3))=1;
% end
% 
% 
% 
% for x=1:Nsize(1)
%     for z=1:Nsize(3)
%         startV=1;
%         prev=1;
%         cnt=0;
%         newPos=zeros(1,3);
%         for y=1:Nsize(2)
%             if vIndex(x,y,z)==0
%                 continue;
%             end
%             if cnt==0
%                 startV=vIndex(x,y,z);
%                 prev=y;
%                 cnt=1;
%                 newPos=NewV(vIndex(x,y,z),:);
%                 continue;
%             end
%             if connectMatrix(vIndex(x,prev,z),vIndex(x,y,z))==1
%                 cnt=cnt+1;
%                 newPos=newPos+NewV(vIndex(x,y,z),:);
%                 NewF(NewF==vIndex(x,y,z))=startV;
%             else
%                 NewV(startV,:)=newPos/cnt;
%                 cnt=0;
%                 newPos=zeros(1,3);
%             end
%             prev=y;
%         end
%         if cnt~=0
%                 NewV(startV,:)=newPos/cnt;
%         end
% 
%     end
% end
% 
% 
% NewF(NewF(:,1)==NewF(:,2)|NewF(:,1)==NewF(:,3)|NewF(:,2)==NewF(:,3),:)=0;
% % NewF=NewF(NewF(:,1)~=0,:);
% 
% tV=NewV*doublingFactor/0.032*2;
% tV(:,1)=round(tV(:,1)-min(tV(:,1))+1);
% tV(:,2)=round(tV(:,2)-min(tV(:,2))+1);
% tV(:,3)=round(tV(:,3)-min(tV(:,3))+1);
%  
% 
% vIndex=zeros(max(tV(:,1)),max(tV(:,2)),max(tV(:,3)));
% 
% Nsize=size(vIndex);
% 
% 
% for i=1:size(NewV,1)
%     vIndex(tV(i,1),tV(i,2),tV(i,3))=i;
% end
% 
% connectMatrix=zeros(size(NewV,1),size(NewV,1));
% for i=1:size(NewF,1)
%     connectMatrix(NewF(i,1),NewF(i,2))=1;
%     connectMatrix(NewF(i,2),NewF(i,1))=1;
%     connectMatrix(NewF(i,2),NewF(i,3))=1;
%     connectMatrix(NewF(i,3),NewF(i,2))=1;
%     connectMatrix(NewF(i,3),NewF(i,1))=1;
%     connectMatrix(NewF(i,1),NewF(i,3))=1;
% end
% 
% 
% 
% 
% for z=1:Nsize(3)
%     for y=1:Nsize(2)
%         startV=1;
%         prev=1;
%         cnt=0;
%         newPos=zeros(1,3);
%         for x=1:Nsize(1)
%             if vIndex(x,y,z)==0
%                 continue;
%             end
%             if cnt==0
%                 startV=vIndex(x,y,z);
%                 prev=x;
%                 cnt=1;
%                 newPos=NewV(vIndex(x,y,z),:);
%                 continue;
%             end
%             if connectMatrix(vIndex(prev,y,z),vIndex(x,y,z))==1
%                 cnt=cnt+1;
%                 newPos=newPos+NewV(vIndex(x,y,z),:);
%                 NewF(NewF==vIndex(x,y,z))=startV;
%             else
%                 NewV(startV,:)=newPos/cnt;
%                 cnt=0;
%                 newPos=zeros(1,3);
%             end
%             prev=x;
%         end
%         
%         if cnt~=0
%                 NewV(startV,:)=newPos/cnt;
%         end
% 
%     end
% end
% 
% NewF(NewF(:,1)==NewF(:,2)|NewF(:,1)==NewF(:,3)|NewF(:,2)==NewF(:,3),:)=0;
% NewF=NewF(NewF(:,1)~=0,:);
