function [Istckmain, IstckA, IstckB, IstckC, IstckD] = FitMainDendriteShaft(Istck1) 

%************************************************************************* 
%This function uses "Istck", TIF binary stack file and estimates the volume 
%occupied by dendritic shaft %The outputs of the function are "Istckmain", the image stack containing 
%both shaft and spines but other voxels removed, "IstckA,B,C,D", the image 
%stacks of dendritic shaft with different diameter estimates. 
%IstckA is the optimized estimate, B to D are with increasing diameters. 
%*************************************************************************
Aver=56;
span=25;
moveval = 30;


%Check voxel size of image 
Nsize = size(Istck1);

%************************************************************************* 
%START Rough estimate of the center of dendritic shaft 
%*************************************************************************

%Make 2D projection and normalize pixel value to 1 
Sthre= sum(Istck1, 3) >0;

%Identify the largest object in 2D projection image 
Objimg = bwareafilt(Sthre,1);

%Create new 3D matrix 
Istck2 = zeros(Nsize(1), Nsize(2), Nsize(3));

%Make 5 times expanded 2D projection template and isolate the 
%original objects that overlap with this template 
for k=1:Nsize(3) 
    Istck2(:,:,k) = Istck1(:,:,k).*bwmorph(Objimg,'thicken',5); 
end
% 
% for k=1:Nsize(3)     
%     Iresult = Istck1(:,:,k);
%     %Output images with "append" mode to generate an image stack    
%     imwrite(Iresult, 'testOutput0.tif', 'WriteMode', 'append', 'Compression','none'); 
% end
% 
% for k=1:Nsize(3)
%     Iresult = Istck2(:,:,k);
%     %Output images with "append" mode to generate an image stack    
%     imwrite(Iresult, 'testOutput.tif', 'WriteMode', 'append', 'Compression','none'); 
% end


%Start obtaining Y and Z values of the dendrite center line with a given X 
%Prepare matrixes for recording positions 
posY = zeros(Nsize(2),1); 
posZ = zeros(Nsize(2),1);  
avposY = zeros(Nsize(2),1);
avposZ = zeros(Nsize(2),1); 
ObjimgYZ = sum(Istck2, 1);



%Fill the gap at the edge of XY projection images 
%Fill gap at the left side 25 pixels    
for  k=1:25 
    if sum(Objimg(:,k))==0 
        Objimg(:,k)=Objimg(:,k+25); 
    end
    if sum(ObjimgYZ(:,k,:))==0
        ObjimgYZ(:,k,:)=ObjimgYZ(:,k+25,:); 
    end

end
%Fill gap at the right side 25 pixels    
for k =Nsize(2)-25:Nsize(2) 
    if sum(Objimg(:,k))==0 
        Objimg(:,k)=Objimg(:,k-25); 
    end
    if sum(ObjimgYZ(:,k,:))==0
        ObjimgYZ(:,k,:)=ObjimgYZ(:,k-25,:); 
    end

end

%Along the X-axis, calculate the center of the shaft     
for k=1:Nsize(2) 
    if sum(Objimg(:,k))>0
        %Along Y axis, calculate the mean of the voxel positions 
        posY(k) = round(mean(find(Objimg(:,k)))); 
    end
    if sum(ObjimgYZ(:,k,:))>0
        %Along Z axis, calculate the mean of the voxel positions 
        posZ(k) = round(mean(find(ObjimgYZ(:,k,:))));
    end
end

%Average the obtained positions of dendrite center line for Aver pixels
% Aver=30
for k=1:Aver 
    avposY(k) = round(mean(posY(1:k+Aver))); 
    avposZ(k) = round(mean(posZ(1:k+Aver)));    
end

for k=Aver+1:Nsize(2)-Aver 
    avposY(k) = round(mean(posY(k-Aver:k+Aver))); 
    avposZ(k) = round(mean(posZ(k-Aver:k+Aver)));    
end

for k=Nsize(2)-Aver+1:Nsize(2) 
    avposY(k) = round(mean(posY(k-Aver:Nsize(2)))); 
    avposZ(k) = round(mean(posZ(k-Aver:Nsize(2))));    
end

%Create 2D image with positive pixels spread from the shaft center line with width of 200 pixels 
%This template is used to isolate the main dendritic shaft and associated spines

%Prepare new 2D image matrix 
Objimg2 = zeros(Nsize(1), Nsize(2)); 
%Put pixel value of 1 within the range of plus/minus 100 pixels from the shaft center line 
for k=1:Nsize(2)    
    if avposY(k) -100 >= 1 && avposY(k) + 100 <= Nsize(1) 
        for l = avposY(k)-100:avposY(k)+100 
            Objimg2(l, k) =1;
        end
    end
    
    if avposY(k) -100 < 1 && avposY(k) + 100 <= Nsize(1) 
        for l = 1:avposY(k)+100 
            Objimg2(l, k) =1; 
        end
    end
    
    if avposY(k) -100 >= 1  && avposY(k) + 101 > Nsize(1) 
        for l = avposY(k)-100:Nsize(1) 
            Objimg2(l, k) =1; 
        end
    end
end
%Prepare new 3D image matrix 
Istckmain = zeros(Nsize(1), Nsize(2), Nsize(3)); 
%From the original 3D image matrix, isolate the volume near the main dendrite 
for k=1:Nsize(3) 
    Istckmain(:,:,k) = Istck1(:,:,k);%.*Objimg2; 
end


%*************************************************************************
%END Rough estimate of the center of dendritic shaft 
%*************************************************************************

%************************************************************************* 
%START Fine fitting of dendritic shaft 
%Estimate dendritic shaft diameter and ellipticity 
%*************************************************************************

%Create a matrix containing ellipses with different diameters (8 -30) and different ellipticity (1:2 to 2:1) 
Elip = zeros(161, 161, 15, 5); 
%Prepare a vector with different radius 
RadialV = [8 9.5 11 12.5 14 15.5 17 18.5 20 21.5 23 24.5 26 27.5 29]; 
%Prepare a vector with different ellipticity 
Round = [1.5 1.25 1 1 1;1 1 1 1.25 1.5];

for k=1:15    
    for l=1:5 
        for j=1:161
            for i=1:161 
                if sqrt( (i-81)*(i-81)/((Round(1,l)*RadialV(k)))^2 + (j-81)*(j-81)/((Round(2,l) *RadialV(k)))^2 ) <= 1
                    Elip(i, j, k, l) = 1; 
                end
            end
        end
    end
end

%Prepare vectors necessary for the next calculations 
Radius = zeros(Nsize(2),1); 
Elliptic = zeros(Nsize(2),1); 
OptY = zeros(Nsize(2),1); 
OptZ = zeros(Nsize(2),1); 


for k=1:Nsize(2)
    MaxLocalR=1;
    OptY(k)=avposY(k);
    for i=-20:20
        for r=MaxLocalR+1:30
            xStart=max(k-span,1);
            xEnd=min(k+span,Nsize(2));
            sum1=0;
            sum2=0;
            for j=xStart:xEnd
                yStart=max(avposY(j)+i-r,1);
                yEnd=min(avposY(j)+i+r,Nsize(1)); 
                sum1=sum1+sum(Objimg(yStart:yEnd, j)>0);
                sum2=sum2+yEnd-yStart+1;
            end                
            if sum1/sum2>0.95
                MaxLocalR=r;
                OptY(k)=avposY(k)+i;
            else
                break;
            end
        end
    end
end

for k=1:Nsize(2)
    MaxLocalR=2;
    OptZ(k)=avposZ(k);
    for i=-20:20
        for r=MaxLocalR+1:30
            xStart=max(k-span,1);
            xEnd=min(k+span,Nsize(2));
            sum1=0;
            sum2=0;
            for j=xStart:xEnd
                zStart=max(avposZ(j)+i-r,1);
                zEnd=min(avposZ(j)+i+r,Nsize(3)); 
                sum1=sum1+sum(squeeze(ObjimgYZ(:,j,zStart:zEnd))>0);
                sum2=sum2+zEnd-zStart+1;
            end                
            if sum1/sum2>0.95
                MaxLocalR=r;
                OptZ(k)=avposZ(k)+i;
            else
                break;
            end
        end
    end
end


%Fit the best Y and Z positions along X axis from 31 to max minus 30 
%Fitting is done by 3D column with thickness of 61    
% 
% for k=1:Nsize(2) 
%     MaxLocal = 0; 
%     OptY(k) = avposY(k); 
%     OptZ(k) = avposZ(k); 
%     for j=-16:16 
%         for i=-16:16 
%             %Isolate cuboid with X value centered at k, spanning plus/minus 30 voxels 
%             %with size of 33 voxels along Y axis and 9 voxels along Z axis 
%             yStart=max(avposY(k)+i-16,1);
%             yEnd=min(avposY(k)+i+16,Nsize(1));
%             xStart=max(k-span,1);
%             xEnd=min(k+span,Nsize(2));
%             zStart=max(avposZ(k)+j-16,1);
%             zEnd=min(avposZ (k)+j+16,Nsize(3));
%             LocalM1 = squeeze(Istck2(yStart:yEnd, xStart:xEnd, zStart:zEnd)); 
%             %Prepare the standard size of column with radius of 16 and length of 61 
%             
%             LocalM2pre = Elip(yStart-avposY(k)-i+81:yEnd-avposY(k)-i+81,zStart-avposZ(k)-j+81:zEnd-avposZ(k)-j+81,5,3); 
%             LocalM2 = permute(repmat(LocalM2pre,[1 1 min(k+span,Nsize(2))-max(k-span,1)+1]), [1 3 2]); 
%             %Calculate voxel numbers with overlap of real shaft and column 
%             %Compare with the previous max value 
%             if MaxLocal < sum(sum(sum(LocalM1.*LocalM2))) 
%                 %Replace MaxLocal with new value 
%                 MaxLocal = sum(sum(sum(LocalM1.*LocalM2))); 
%                 %Position of (Y, Z) corresponding to the center of the shaft is also renewed 
%                 OptY(k) = avposY(k)+i; 
%                 OptZ(k) = avposZ(k)+j; 
%             end
%         end
%     end
% end


%Check the position along X axis where the center is erroneously shifted toward large spines
OptYNew = OptY; 
%Prepare differential matrix of Y position     
OptYmeandiff = diff(OptY);

%Prepare a vector for judgement of anomalous positions     
judge = zeros(Nsize(2));     
for k=1:Nsize(2)-41 
    %Find the gap position where Y values change more than 10 
    if abs(OptYmeandiff(k)) > 10 && judge (k+1) == 0 
        %Within 40 voxels from this position, find the second position with gap in opposite direction 
        judgekk = 0; 
        for kk = 1:40 
            if OptYmeandiff(k)*OptYmeandiff(k+kk) < 0 && abs(OptYmeandiff(k+kk)) > 10 && judgekk == 0 
                %If the second gap is found, the Y value between two gaps is set to the average of two points 
                %before and after the two gaps 
                OptYNew(k+1:k+1+kk) = floor((OptY(k) + OptY(k+kk+2))/2); 
                %Set judge vector to be 1 from the first voxel to the 
                %second gap 
                judge(1:k+kk+1) = 1;
                %Set judgekk parameter to be 1 to avoid finding the third gap 
                judgekk = 1; 
            end
        end
    end
end



%Check the position along X axis where the center is erroneously shifted toward large spines
OptZNew = OptZ; 
%Prepare differential matrix of Y position     
OptZmeandiff = diff(OptZ);

%Prepare a vector for judgement of anomalous positions     
judge = zeros(Nsize(2));     
for k=1:Nsize(2)-41 
    %Find the gap position where Y values change more than 10 
    if abs(OptZmeandiff(k)) > 10 && judge (k+1) == 0 
        %Within 40 voxels from this position, find the second position with gap in opposite direction 
        judgekk = 0; 
        for kk = 1:40 
            if OptZmeandiff(k)*OptZmeandiff(k+kk) < 0 && abs(OptZmeandiff(k+kk)) > 10 && judgekk == 0 
                %If the second gap is found, the Y value between two gaps is set to the average of two points 
                %before and after the two gaps 
                OptZNew(k+1:k+1+kk) = floor((OptZ(k) + OptZ(k+kk+2))/2); 
                %Set judge vector to be 1 from the first voxel to the 
                %second gap 
                judge(1:k+kk+1) = 1;
                %Set judgekk parameter to be 1 to avoid finding the third gap 
                judgekk = 1; 
            end
        end
    end
end

OptZNew = round(movmean(OptZNew, moveval));  
OptYNew = round(movmean(OptYNew, moveval));  
% 
% 
% f1=figure;
% imshow(permute(squeeze(ObjimgYZ),[2 1]))
% hold on
% plot(1:Nsize(2),OptZNew)
% plot(1:Nsize(2),avposZ)
% axis([1 Nsize(2) 1 Nsize(3)])
% hold off
% 
% f2=figure;
% imshow(Objimg)
% hold on
% plot(1:Nsize(2),OptYNew)
% plot(1:Nsize(2),avposY)
% axis([1 Nsize(2) 1 Nsize(1)])
% hold off

%In the previous part, Y and Z positions are estimated 
%In the next section, optimal radius and ellipticity are estimated  
for k=1:Nsize(2) 
    MaxLocalSize = 0; 
    Radius(k) = 2; 
    Elliptic (k) = 1; 
    for j=1:11
        for i=1:5 
            %Isolate the plane with X value of k, spanning 161 voxels along Y axis 
            %and 41 voxels along Z axis 
            LocalM1 = squeeze(Istck2(max(OptYNew(k)-80,1):min(OptYNew(k)+80,Nsize(1)), k, max(OptZNew(k)-80,1):min(OptZNew(k)+80,Nsize(3)))); 
            LocalM2 = Elip(max(OptYNew(k)-80,1)-OptYNew(k)+81:min(OptYNew(k)+80,Nsize(1))-OptYNew(k)+81,max(OptZNew(k)-80,1)-OptZNew(k)+80+1:min(OptZNew(k)+80,Nsize(3))-OptZNew(k)+80+1,j,i); 
            LocalSize = sum(sum(LocalM1.*LocalM2)); 
            LocalRatio = LocalSize/sum(LocalM2(:)); 
            %If overlap is larger than the previous trial and the ratio is more than 0.6, 
            %replace the recorded radius and ellipticity with new values 
            if MaxLocalSize < LocalSize && LocalRatio > 0.6 
                MaxLocalSize = LocalSize; 
                Radius(k) = j+1; 
                Elliptic (k) = i; 
            end
        end
    end
end

 %Smoothing the shaft shape by averaging Y position, radius, and ellipticity
%  moveval = 31;
%  OptZNew = round(movmean(OptZNew, moveval));  
%  OptYNew = round(movmean(OptYNew, moveval));  
 Radius = round(movmean(Radius, moveval));  
 Elliptic = round(movmean(Elliptic, moveval));
  
 %Prepare 3D matrixes for different shaft 3D data
 Istck3A = zeros(Nsize(1), Nsize(2), Nsize(3)); 
 Istck3B = zeros(Nsize(1), Nsize(2), Nsize(3));  
 Istck3C = zeros(Nsize(1), Nsize(2), Nsize(3));  
 Istck3D = zeros(Nsize(1), Nsize(2), Nsize(3));
 
 %Create 3D models of dendritic shaft with different radii 
 for k=1:Nsize(2)    
     %Create 3D model of dendritic shaft with best-fit radius     
     Istck3A(max(OptYNew(k)-80,1):min(OptYNew(k)+80,Nsize(1)), k, max(OptZNew(k)-80,1):min(OptZNew(k)+80,Nsize(3))) = Elip(max(OptYNew(k)-80,1)-OptYNew(k)+81:min(OptYNew(k)+80,Nsize(1))-OptYNew(k)+81,max(OptZNew(k)-80,1)-OptZNew(k)+80+1:min(OptZNew(k)+80,Nsize(3))-OptZNew(k)+80+1,Radius(k),Elliptic(k));    
     %Create 3D model of dendritic shaft with radius one step larger than     
     %the best fit     
     Istck3B(max(OptYNew(k)-80,1):min(OptYNew(k)+80,Nsize(1)), k, max(OptZNew(k)-80,1):min(OptZNew(k)+80,Nsize(3))) = Elip(max(OptYNew(k)-80,1)-OptYNew(k)+81:min(OptYNew(k)+80,Nsize(1))-OptYNew(k)+81,max(OptZNew(k)-80,1)-OptZNew(k)+80+1:min(OptZNew(k)+80,Nsize(3))-OptZNew(k)+80+1,Radius(k)+1,Elliptic(k));    
     %Create 3D model of dendritic shaft with radius two step larger than     
     %the best fit    
     Istck3C(max(OptYNew(k)-80,1):min(OptYNew(k)+80,Nsize(1)), k, max(OptZNew(k)-80,1):min(OptZNew(k)+80,Nsize(3))) = Elip(max(OptYNew(k)-80,1)-OptYNew(k)+81:min(OptYNew(k)+80,Nsize(1))-OptYNew(k)+81,max(OptZNew(k)-80,1)-OptZNew(k)+80+1:min(OptZNew(k)+80,Nsize(3))-OptZNew(k)+80+1,Radius(k)+2,Elliptic(k));    
     %Create 3D model of dendritic shaft with radius three step larger than    
     %the best fit     
     Istck3D(max(OptYNew(k)-80,1):min(OptYNew(k)+80,Nsize(1)), k, max(OptZNew(k)-80,1):min(OptZNew(k)+80,Nsize(3))) = Elip(max(OptYNew(k)-80,1)-OptYNew(k)+81:min(OptYNew(k)+80,Nsize(1))-OptYNew(k)+81,max(OptZNew(k)-80,1)-OptZNew(k)+80+1:min(OptZNew(k)+80,Nsize(3))-OptZNew(k)+80+1,Radius(k)+3,Elliptic(k));    
 end
 
%Pass through median filter to further remove local misalignment
val=5;
IstckA = medfilt3(Istck3A,[val val val]); 
IstckB = medfilt3(Istck3B,[val val val]); 
IstckC = medfilt3(Istck3C,[val val val]); 
IstckD = medfilt3(Istck3D,[val val val]);
% 
% for k=1:Nsize(3)    
%     Iresult3 = IstckA(:,:,k);
%     %Output images with "append" mode to generate an image stack    
%     imwrite(Iresult3, 'istck3amed2.tif', 'WriteMode', 'append', 'Compression','none'); 
% end

% IstckA=Istck3A;
% IstckB=Istck3B;
% IstckC=Istck3C;
% IstckD=Istck3D;

%************************************************************************* 
%END Fine fitting of dendritic shaft 
%Estimate dendritic shaft diameter and ellipticity 
%************************************************************************* 



