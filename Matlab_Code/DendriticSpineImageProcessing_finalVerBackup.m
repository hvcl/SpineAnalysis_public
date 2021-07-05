function DendriticSpineImageProcessing(inputPath,resultPath)
    % path param
    fname = inputPath; %tif file ex) D:/1.tif
     %folder ex) D:/dataset1/
    if ~isfolder(resultPath)
        mkdir(resultPath);
    end
    %active contour param
    AcSmoothFactor=1;
    AcContractionBias=0.3; %shrink amount
    AcInitThresWeight=1.2;
    
    zscaleFactor=0.12/0.032;
    doublingFactor=1;
    LocalDoublingFactor=2;
    zSmoothRange=5;
    xySmoothRange=3;
    zSmoothThresh=0.5;
    
    smoothingItt=2;
    
    doDenoising=0;

    % Obtain information of image file
    info = imfinfo(fname);
    num_images = numel(info);


    for k=1:num_images
        imagebuffer = imread(fname, k, 'Info', info);
        if k==1
            buffer = imagebuffer;
        else
            buffer(:,:,k) = imagebuffer;
        end
    end
    clear imagebuffer
    if size(buffer,1)>size(buffer,2)
        buffer=permute(buffer,[2,1,3]);
    end

    [ysize,xsize,zsize]=size(buffer);
    %buffer=double(meshMatrix)/double(max(meshMatrix(:)));
    %buffer = imgaussfilt3(buffer, 2.0);

    
%     if doDenoising
%         net = denoisingNetwork('DnCNN');
%         for k = 1:size(buffer,3)
%             k
%             buffer(:,:,k) = denoiseImage(buffer(:,:,k),net);%wdenoise2(res1(:,:,k));%imgaussfilt3(buffer, sig);
%             buffer(:,:,k) = wdenoise2(buffer(:,:,k));
%         end
%     end
%    
%     for k = 1:size(buffer,3)
%         thresh = multithresh(buffer(:,:,k),2);
%         %the smaller value of two thresholds is stored in "stthresh"
%         stthresh (k) = thresh(1);
%     end
%  
%     stthreshB = sort(stthresh, 'descend');
%     %The first three elements are selected and averaged to be
%     %used as a common threshold for all images
%     avestthreshB = mean(stthreshB(1:3));

    buffer=double(buffer)/double(max(buffer(:)));
    % interpolation _x4
%    [nY,nX,nZ]=meshgrid(1:ysize,1:xsize,1:0.032/0.12:zsize);
    %res1 = interp3(buffer,nX,nY,nZ,'linear');
    res1=imresize3(buffer,[ysize,xsize,round(zsize*zscaleFactor*doublingFactor*LocalDoublingFactor)],'linear');
    res1=uint16(res1*(2^16-1));

    %res1=buffer;
    clear buffer
%    clear nY
%    clear nZ
%    clear nX
    

    if doDenoising
        %res1=permute(res1,[3,2,1]);
        for k=1:size(res1,3)
            disp(k);
            res1(:,:,k) =wdenoise2(res1(:,:,k)); %denoiseImage(res1(:,:,k),net);%wdenoise2(res1(:,:,k));%imgaussfilt3(buffer, sig);
        end
        %res1=permute(res1,[3,2,1]);
    end

    %% 2. make a binary file (active contour)
    Nsize=size(res1);
    
    for k = 1:size(res1,3)
        thresh = multithresh(res1(:,:,k),2);
        %the smaller value of two thresholds is stored in "stthresh"
        stthresh (k) = thresh(1);
    end
 
    stthreshB = sort(stthresh, 'descend');
    %The first three elements are selected and averaged to be
    %used as a common threshold for all images
    avestthreshB = mean(stthreshB(1:3));
    
    
    
    %Create a new matrix "Ithresh"
    Ithresh = zeros(Nsize(1), Nsize(2));
    %Declare the file name of the output image stack
    for k=1:Nsize(3)
        %kth image in the stack is stored in the matrix "imagebuffer"
        imagebuffer2 = res1(:,:,k);
        %Select pixels with intensities higher than
        %threshold "avestthresB" multiplied by a coefficient
        %Coefficient is 1.1 for GFP (1.7 for DiI)
        Ithresh = imagebuffer2 > avestthreshB*AcInitThresWeight;
        %4th parameter is either 'edge' or 'Chan-Vese'
        if k==1
            %buffer=Ithresh;
            buffer = activecontour(imagebuffer2, Ithresh, 20, 'edge');
        else
            %buffer(:,:,k)=Ithresh;
            buffer(:,:,k) = activecontour(imagebuffer2, Ithresh, 20, 'edge');
        end
    end
    res2=buffer;
    if zSmoothRange>1
        res2=permute(res2,[3,2,1]);
        for slice=1:size(res2,3)
            if slice==1
                imwrite(res2(:,:,slice),[resultPath,'prevZsmooth.tif'] ,'tif','Compression', 'none');
            else
                imwrite(res2(:,:,slice),[resultPath,'prevZsmooth.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
            end
        end
        res2=permute(res2,[3,2,1]);
        res2=medfilt3(res2,[xySmoothRange,xySmoothRange,zSmoothRange]);
        res2=permute(res2,[3,2,1]);
        %res2=movmedian(res2,zSmoothRange,1);
        for slice=1:size(res2,3)
            if slice==1
                imwrite(res2(:,:,slice),[resultPath,'afterZsmooth.tif'] ,'tif','Compression', 'none');
            else
                imwrite(res2(:,:,slice),[resultPath,'afterZsmooth.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
            end
        end
        res2=permute(res2,[3,2,1]);
    end
    
    
    if LocalDoublingFactor>1
        res2=imresize3(double(res2),[Nsize(1),Nsize(2),round(Nsize(3)/LocalDoublingFactor)],'linear');
        res2=res2>zSmoothThresh;
        res1=imresize3(res1,[Nsize(1),Nsize(2),round(Nsize(3)/LocalDoublingFactor)],'linear');
    end


    %% for debug
    %tres=permute(res1,[3,2,1]);
    %for slice=1:size(tres,3)
    %    imwrite(tres(:,:,slice),[resultPath,'data_xz.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    %end
    for slice=1:size(res1,3)
        if slice==1
            imwrite(res1(:,:,slice),[resultPath,'data.tif'] ,'tif','Compression', 'none');
        else
            imwrite(res1(:,:,slice),[resultPath,'data.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
        end
    end
    
    t=permute(res2,[3,2,1]);
    for slice=1:size(t,3)
        if slice==1
            imwrite(t(:,:,slice),[resultPath,'binary_xz.tif'] ,'tif','Compression', 'none');
        else
            imwrite(t(:,:,slice),[resultPath,'binary_xz.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
        end
    end
    clear t
    clear imagebuffer2
    clear Ithresh
    clear buffer
    clear res1
    % 
    % [ysize,xsize,zsize]=size(res2);
    % [nY,nX,nZ]=meshgrid(1:ysize,1:xsize,1:0.032/0.016:zsize);
    % res1=double(res1)/double(max(res1(:)));
    % res1 = interp3(res1,nY,nX,nZ);
    % res1=uint16(res1*(2^16-1));
    % res2 = interp3(double(res2),nY,nX,nZ);
    % 
    % tres=permute(res1,[1,2,3]);
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice),[resultPath,'data.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end

    %% for debug
    % tres=permute(res2,[3,2,1]);
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice),[resultPath,'binary_xz.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end
    % tres=permute(res2,[1,2,3]);
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice),[resultPath,'binary_xy.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end


    %% Spine Detection
    [Istck2, Istck3A, Istck3B, Istck3C, Istck3D,fig1,fig2] = FitMainDendriteShaft(res2,doublingFactor);
    saveas(fig1,[resultPath,'z_fitline.jpg']);
    saveas(fig2,[resultPath,'y_fitline.jpg']);
    dendrite=Istck3A;

    %Isolate spines by subtracting shaft voxels from the dendrite voxels 
    Istck4 = (Istck2-dendrite) >0; 

    %Identify voxel clusters corresponding to single spine candidate 
    Istck5 = bwlabeln(Istck4,26); 
    clear Istck4
    
    
    t=max(Istck5(:))+1;
    tres=Istck5 + Istck3A*t;
    for slice=1:doublingFactor:size(tres,3)
        if slice==1
            imwrite(tres(:,:,slice)/t,[resultPath,'spineWithDendrite_original.tif'] ,'tif','Compression', 'none');
        else        
            imwrite(tres(:,:,slice)/t,[resultPath,'spineWithDendrite_original.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
        end
    end
    clear t
    clear tres
    imwrite(labeloverlay(sum(res2,3)/max(max(sum(res2,3))),max(Istck5,[],3)),[resultPath,'initialDetect.jpg']);

    

    %Calculate number of spine candidates 
    MaxN = max(Istck5(:)); 

    %************************************************************************* 
    %START remove large residual voxels around dendritic shaft 
    %************************************************************************* 

    for k=1:MaxN 
        %Pick up kth spine candidate    
        CheckM = Istck5==k;     
        VoxelNo = sum(CheckM(:)); 
        if VoxelNo > 20000 *doublingFactor
            %If voxel size is more than 5000, start evaluate overlap with shaft 
            %CheckMB,C,D are overlap with shaft volume with different radius 
            CheckMB = (CheckM.*Istck3B) >0; 
            CheckMC = (CheckM.*Istck3C) >0; 
            CheckMD = (CheckM.*Istck3D) >0; 
            VoxelNoB = sum(sum(sum(CheckMB))); 
            VoxelNoC = sum(sum(sum(CheckMC))); 
            VoxelNoD = sum(sum(sum(CheckMD))); 
            %If overlap is large with three shaft models, remove the overlap 

            if VoxelNoB > 2000*doublingFactor && (VoxelNo - VoxelNoB) > 392*doublingFactor 
                Istck5 = Istck5-k*CheckMB; 
                dendrite=dendrite+CheckMB;        
            end
            if VoxelNoB < 2000*doublingFactor && VoxelNoC > 2000*doublingFactor && (VoxelNo - VoxelNoC) > 392*doublingFactor 
                Istck5 = Istck5-k*CheckMC; 
                dendrite=dendrite+CheckMC;
            end
            if VoxelNoB < 2000*doublingFactor && VoxelNoC < 2000*doublingFactor &&VoxelNoD > 2000*doublingFactor && (VoxelNo - VoxelNoD)> 392*doublingFactor 
                Istck5 = Istck5-k*CheckMD; 
                dendrite=dendrite+CheckMD;
            end

        end
    end
    
    clear Istck3A

    
    se1 = strel('sphere',1);
    se2 = strel('sphere',2);

    for k=1:MaxN 
        %Pick up kth spine candidate    
        CheckM = Istck5==k;     
        Junction1 = imdilate(CheckM, se1).*dendrite;

        CheckMB = (CheckM.*Istck3B) >0; 
        CheckMC = (CheckM.*Istck3C) >0; 
        CheckMD = (CheckM.*Istck3D) >0; 
        
%         CheckMB=(imdilate(Junction1,se1).*CheckM)>0;
%         CheckMB=imdilate(CheckMB,se1).*CheckM;
%         CheckMC=imdilate(CheckMB,se1).*CheckM;
%         CheckMC=imdilate(CheckMC,se1).*CheckM;
%         CheckMD=imdilate(CheckMC,se1).*CheckM;
%         CheckMD=imdilate(CheckMD,se1).*CheckM;

        VoxelNoB = sum(CheckMB(:)); 
        VoxelNoC = sum(CheckMC(:))-VoxelNoB; 
        VoxelNoD = sum(CheckMD(:))-VoxelNoB-VoxelNoC; 

        if VoxelNoB>1.5 * VoxelNoC
            Istck5 = Istck5-k*CheckMC; 
    %        dendrite=dendrite+CheckMB;  
        elseif VoxelNoC>1.5 * VoxelNoD
            Istck5 = Istck5-k*CheckMD; 
    %        dendrite=dendrite+CheckMC;
        end    
    end
    clear Istck3B
    clear Istck3C
    clear Istck3D
    clear CheckMC
    clear CheckMD
    
    Istck5 = bwlabeln(Istck5,26); 
    MaxN = max(Istck5(:));
    dendrite=Istck2-(Istck5>0);
    for k=1:MaxN
        CheckM = Istck5==k;     
        Junction1 = imdilate(CheckM, se1).*dendrite;
        if sum(Junction1(:))==0
            Istck5(CheckM)=0;
        end
    end
    clear Junction1
    Istck5 = bwlabeln(Istck5,26); 
    
    imwrite(labeloverlay(sum(res2,3)/max(max(sum(res2,3))),max(Istck5,[],3)),[resultPath,'refineSpine1.jpg']);
    
    MaxN = max(Istck5(:));
    dendrite=Istck2-(Istck5>0);
    for k=1:MaxN 
        k
        %Pick up kth spine candidate    
        CheckM = Istck5==k;     
        CheckMA = imdilate(dendrite, se2).*CheckM;
        CheckMB=imdilate(CheckMA,se2).*dendrite;
        VoxelNoA = sum(CheckMA(:)); 
        VoxelNoB = sum(CheckMB(:)); 
        if VoxelNoA==0
            continue;
        end
        while VoxelNoB<VoxelNoA
            VoxelNoB
            VoxelNoA
            Istck5=Istck5+k*CheckMB;
            dendrite=dendrite-CheckMB;
            CheckMA=CheckMB;
            VoxelNoA=VoxelNoB;
            CheckMB=imdilate(CheckMA,se2).*dendrite;
            VoxelNoB=sum(CheckMB(:));
        end
    end
    clear CheckMB
    clear CheckM
    clear CheckMA
    [Istck6, newK] = RefineSpineLocation(Istck5, MaxN,doublingFactor);

    imwrite(labeloverlay(sum(res2,3)/max(max(sum(res2,3))),max(Istck5,[],3)),[resultPath,'refineSpine2.jpg']);
    clear Istck5

    
    dendrite=Istck2-(Istck6>0);
    clear Istck2

    IstckJunction = imdilate(Istck6, se1).*dendrite; 

    for i=1:newK
        tJunction=bwlabeln(IstckJunction==i,26);
        maxcnt=0;
        ind=0;
        for j=1:max(tJunction(:))
            t=tJunction==j;
            cnt=sum(t(:));
            if cnt>maxcnt
                ind=j;
                maxcnt=cnt;
            end
        end
        if ind==0
            continue;
        end
        for j=1:max(tJunction(:))
            if j~=ind
                IstckJunction(tJunction==j)=0;
            end
        end
    end
    clear tJunction

    %% Debug
    % tres=Istck6;
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice)/newK,[resultPath,'spine.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end

    % tres=permute(dendrite,[1,2,3]);
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice),[resultPath,'dendrite_xy.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end
    % 
    % tres=permute(dendrite,[3,2,1]);
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice),[resultPath,'dendrite_xz.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end
    % 
    % tres=Istck6 + dendrite*(newK+1);
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice)/(newK+1),[resultPath,'spineWithDendrite.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end
    % 
    % 
    % tres=permute(Istck6 + dendrite*(newK+1),[3,2,1]);
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice)/(newK+1),[resultPath,'spineWithDendrite_xz.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end
    % 
    % t=IstckJunction;
    % t(t~=0)=t(t~=0)+newK;
    % tres=Istck6+t;
    % for slice=1:size(tres,3)
    %     imwrite(tres(:,:,slice)/(2*newK),[resultPath,'spineWithJunction.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end

    tres=Istck6 + dendrite*(newK+1);
    for slice=1:doublingFactor:size(tres,3)
        if slice==1
            imwrite(tres(:,:,slice)/(newK+1),[resultPath,'spineWithDendrite.tif'] ,'tif','Compression', 'none');
        else
            imwrite(tres(:,:,slice)/(newK+1),[resultPath,'spineWithDendrite.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
        end
    end
    imwrite(labeloverlay(sum(res2,3)/max(max(sum(res2,3))),max(Istck6,[],3)),[resultPath,'finalSpine.jpg']);
    tres=IstckJunction;
    tres(tres~=0)=tres(tres~=0)+newK;
    imwrite(labeloverlay(sum(res2,3)/max(max(sum(res2,3))),max(Istck6+tres,[],3)),[resultPath,'spineWithJunction.jpg']);
    clear res2

    % for slice=1:size(Istck6,3)
    %     imwrite(uint8(Istck6(:,:,slice)+t(:,:,slice)),[resultPath,'spineWithJunction.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
    % end


    tres=Istck6 + dendrite*(newK+1);
    for slice=1:doublingFactor:size(tres,3)
        if slice==1
            imwrite(uint8(tres(:,:,slice)),[resultPath,'label.tif'] ,'tif', 'Compression', 'none');
        else
            imwrite(uint8(tres(:,:,slice)),[resultPath,'label.tif'] ,'tif', 'WriteMode', 'append','Compression', 'none');
        end
    end
    
    clear tres
    clear dendrite



    mkdir([resultPath,'spine']);
    for k = 1:newK 
        k
        %Calculate mesh from isolated spine candidates    
        [VNo, NewN, NewF, NewV]= SpineMeshCalc(Istck6, IstckJunction, k,doublingFactor);    
        %Eliminate isolated mesh group    
        [tri,v] = DeleteIsolateSurface(NewF, NewV); 
        if size(tri,1)<10
            disp('unkwon error1');
            [VNo, NewN, NewF, NewV]= SpineMeshCalc(Istck6, IstckJunction, k,doublingFactor);    
            %Eliminate isolated mesh group    
            [tri,v] = DeleteIsolateSurface(NewF, NewV); 
            if size(tri,1)<10
                disp('unkwon error2');
                continue;
            end
        end
        %% preprocessing: mesh correction
        cCnt=0;
        for i=1:size(v,1)
            for j=i+1:size(v,1)
                if norm(v(i,:)-v(j,:))<0.0001
                    [r,c]=find(tri(:,:)==j);
                    tri(r,c)=i;
                    cCnt=cCnt+1;
                end
            end
        end

        disp("Correction number of vertex is "+cCnt)


        check=zeros(size(v,1),'int8');
        nn=zeros(size(v,1),1);
        neighbor=[];
        for i=1:size(tri,1)
            if check(tri(i,1),tri(i,2))==0
                nn(tri(i,1))=nn(tri(i,1))+1;
                nn(tri(i,2))=nn(tri(i,2))+1;
                check(tri(i,1),tri(i,2))=1;
                check(tri(i,2),tri(i,1))=1;
                neighbor(tri(i,1),nn(tri(i,1)))=tri(i,2);
                neighbor(tri(i,2),nn(tri(i,2)))=tri(i,1);
            end
            if check(tri(i,1),tri(i,3))==0
                nn(tri(i,1))=nn(tri(i,1))+1;
                nn(tri(i,3))=nn(tri(i,3))+1;
                check(tri(i,1),tri(i,3))=1;
                check(tri(i,3),tri(i,1))=1;
                neighbor(tri(i,1),nn(tri(i,1)))=tri(i,3);
                neighbor(tri(i,3),nn(tri(i,3)))=tri(i,1);
            end
            if check(tri(i,3),tri(i,2))==0
                nn(tri(i,3))=nn(tri(i,3))+1;
                nn(tri(i,2))=nn(tri(i,2))+1;
                check(tri(i,3),tri(i,2))=1;
                check(tri(i,2),tri(i,3))=1;
                neighbor(tri(i,3),nn(tri(i,3)))=tri(i,2);
                neighbor(tri(i,2),nn(tri(i,2)))=tri(i,3);
            end
        end

        check=zeros(size(v,1),1,'int8');
        n=3;
        vn=3;
        stack(1:3)=tri(1,1:3);
        check(tri(1,1:3))=1;
        while n~=0
            cur_v=stack(n);
            n=n-1;
            t=neighbor(cur_v,check(neighbor(cur_v,1:nn(cur_v)))==0);
            check(t)=1;
            stack(n+1:n+length(t))=t;
            n=n+length(t);
            vn=vn+length(t);
        end
        if vn~=size(v,1)
            disp('-----------------------------------------------------------------------')
            disp([num2str(k),': The mesh have multiple connected component'])
            continue;
        end


        CountN = zeros(size(v,1),'int8');
        for i = 1:size(tri,1)
            CountN(tri(i,1),tri(i,2))=CountN(tri(i,1),tri(i,2))+1;
            CountN(tri(i,1),tri(i,3))=CountN(tri(i,1),tri(i,3))+1;
            CountN(tri(i,2),tri(i,3))=CountN(tri(i,2),tri(i,3))+1;
            CountN(tri(i,2),tri(i,1))=CountN(tri(i,2),tri(i,1))+1;
            CountN(tri(i,3),tri(i,2))=CountN(tri(i,3),tri(i,2))+1;
            CountN(tri(i,3),tri(i,1))=CountN(tri(i,3),tri(i,1))+1;
        end
        [t1,t2]=find(CountN(:,:)==1);

        if length(t1)==0
            disp('-----------------------------------------------------------------------')
            disp([num2str(k),': The mesh does not have open area'])
            continue;
        end

        check=zeros(size(v,1),1,'int8');
        check(t1)=1;
        check(t2)=1;

        n=2;
        stack(1)=t1(1);
        check(t1(1))=0;
        stack(2)=t2(1);
        check(t2(1))=0;
        while n~=0
            cur_v=stack(n);
            n=n-1;
            t=neighbor(cur_v,check(neighbor(cur_v,1:nn(cur_v)))==1);
            check(t)=0;
            stack(n+1:n+length(t))=t;
            n=n+length(t);
        end
        if length(find(check(:)==1))>0
            disp('-----------------------------------------------------------------------')
            disp([num2str(k),': The mesh have multiple open area'])
            continue;
        end

        %% smoothing
        % smooth the mesh using curvature flow smoothing(mode=1) or 
        %laplacian smoothing with inverse vertice distance based umbrella weights (mode=0)
        clear mex
        FV.vertices=v;
        FV.faces=tri;
        FV=smoothpatch(FV,1,smoothingItt,1);
        v=FV.vertices;
        tri=FV.faces;
        plywrite([resultPath,'spine/',num2str(k),'.ply'],tri,v);


        %% get curvatures
        %mean and gaussian curvatature
        getderivatives=0;
        [PrincipalCurvatures,PrincipalDir1,PrincipalDir2,FaceCMatrix,VertexCMatrix,Cmagnitude]= GetCurvatures( FV ,getderivatives);

        GausianCurvature=PrincipalCurvatures(1,:).*PrincipalCurvatures(2,:);
        MeanCurvature=(PrincipalCurvatures(1,:)+PrincipalCurvatures(2,:))/2;

        TriS=tri;
        PtsS=v;
        [rS, cS] = size(TriS);
        [rPts, ~] = size(PtsS);
        PtsV = PtsS;
        TriV = TriS;
        CountN = int8(zeros(rPts, rPts));
        SignMatrix = int8(zeros(rPts, rPts));
        for kk = 1:rS
            if TriS(kk,1) < TriS(kk,2)
                CountN(TriS(kk,1), TriS(kk,2)) =CountN(TriS(kk,1), TriS(kk,2)) + 1;
                SignMatrix(TriS(kk,1), TriS(kk,2)) =1;
            else
                CountN(TriS(kk,2), TriS(kk,1)) =CountN(TriS(kk,2), TriS(kk,1)) + 1;
                SignMatrix(TriS(kk,2), TriS(kk,1)) =-1;
            end
            if TriS(kk,2) < TriS(kk,3)
                CountN(TriS(kk,2), TriS(kk,3)) =CountN(TriS(kk,2), TriS(kk,3)) + 1;
                SignMatrix(TriS(kk,2), TriS(kk,3)) =1;
            else
                CountN(TriS(kk,3), TriS(kk,2)) =CountN(TriS(kk,3), TriS(kk,2)) + 1;
                SignMatrix(TriS(kk,3), TriS(kk,2)) =-1;
            end
            if TriS(kk,3) < TriS(kk,1)
                CountN(TriS(kk,3), TriS(kk,1)) =CountN(TriS(kk,3), TriS(kk,1)) + 1;
                SignMatrix(TriS(kk,3), TriS(kk,1)) =1;
            else
                CountN(TriS(kk,1), TriS(kk,3)) =CountN(TriS(kk,1), TriS(kk,3)) + 1;
                SignMatrix(TriS(kk,1), TriS(kk,3))=-1;
            end
        end
        [select1,select2] = find(CountN ==1);
        MeanCurvature(1,select1)=0;
        MeanCurvature(1,select2)=0;
        GausianCurvature(1,select1)=0;
        GausianCurvature(1,select2)=0;

        output = fopen([resultPath,'spine/',num2str(k),'.plycurve'],'w');
        for i=1:size(MeanCurvature,2)
            fprintf(output,'%f %f\n',MeanCurvature(1,i),GausianCurvature(1,i));
        end
        fclose(output);


        %% Okaba's feature generation
        [rsize, csize] = size([select1,select2] );
        directedMat = [select1,select2];
        if rsize > 0
            for kk = 1:rsize
                if SignMatrix(directedMat(kk,1), directedMat(kk,2)) ==1
                    directedMat(kk,1) = select2(kk);
                    directedMat(kk,2) = select1(kk);
                end
            end
            openV = reshape([select1,select2], [1, rsize*2]);
            reducedV = unique(openV);
            [reducedVNr,reducedVN] = size (reducedV);
            openCenter =[0,0,0];
            for kk = 1:reducedVN
                openCenter = openCenter + PtsS(reducedV(kk), :);
            end
            openPoint = openCenter / reducedVN;
            PtsV = [PtsS; openPoint];
            openPointV = ones(rsize,1)*(rPts+1);
            baseMat = [directedMat openPointV];
            TriV = [TriS; baseMat];
        end
        [rV, cV] = size(TriV);
        %AreaSum:Spine surface area
        AreaSum = 0;
        for kk = 1:rS
            AS = PtsS(TriS(kk,2),:)-PtsS(TriS(kk,1),:);
            BS = PtsS(TriS(kk,3),:)-PtsS(TriS(kk,1),:);
            CS = cross(AS, BS);
            S = 0.5*sqrt(dot(CS, CS));
            AreaSum = AreaSum + S;
        end
        %VolSum:Spine volume
        VolSum = 0;
        for kk = 1:rV
            AV = -PtsV(TriV(kk,3),1)*PtsV(TriV(kk,2),2)*PtsV(TriV(kk,1),3);
            BV = PtsV(TriV(kk,2),1)*PtsV(TriV(kk,3),2)*PtsV(TriV(kk,1),3);
            CV = PtsV(TriV(kk,3),1)*PtsV(TriV(kk,1),2)*PtsV(TriV(kk,2),3);
            DV = -PtsV(TriV(kk,1),1)*PtsV(TriV(kk,3),2)*PtsV(TriV(kk,2),3);
            EV = -PtsV(TriV(kk,2),1)*PtsV(TriV(kk,1),2)*PtsV(TriV(kk,3),3);
            FV_v = PtsV(TriV(kk,1),1)*PtsV(TriV(kk,2),2)*PtsV(TriV(kk,3),3);
            VolSum = VolSum + (AV+BV+CV+DV+EV+FV_v)/6;
        end
        Volume = abs(VolSum);
        %Calculation of convex hull by using function "convhulln"
        [K, vv] = convhulln(PtsV);
        convexVol = vv;
        Ratio = (convexVol - Volume)/Volume;
        %distance:START
        lastMat = TriV(rS+1:rV,:);
        baseV = reshape(lastMat, [1, (rV-rS)*3]);
        uniqueV = unique(baseV);
        [rr,cc] = size(uniqueV);
        baseCenter =[0,0,0];
        for kk = 1:cc
            baseCenter = baseCenter + PtsV(uniqueV(kk), :);
        end
        basePoint = baseCenter / cc;
        [rPts, cPts] = size(PtsS);
        PtsBase = repmat(basePoint, rPts, 1);
        Distance = sqrt(dot((PtsS-PtsBase),(PtsS-PtsBase), 2));
        Average = mean(Distance, 1);
        %Calculate difference of each Distance from Average
        Diff = Distance - Average;
        %Calculate CV of "Distance" from the average value
        CV = mean(Diff.*Diff).^0.5/Average;
        Sorted = sort(Distance);
        StartNum = int16(rPts*0.95);
        Fivepercent = Sorted (StartNum:rPts);
        Spinelength = mean(Fivepercent, 1);
        %Calculation of open angle:START
        DisMat =PtsS-PtsBase;
        Axis = sum(DisMat)/rPts;
        AxisMat = repmat(Axis, rPts, 1);
        Cosvalue = dot(AxisMat, DisMat, 2)./sqrt(dot(AxisMat, AxisMat, 2).*dot(DisMat, DisMat, 2));
        Angle = acos(Cosvalue);
        %Calculate the average of "Angle" for all vertices
        AveVal = mean(Angle, 1);
        Features(k,1:5)={[num2str(k),'.ply'], Volume, Ratio,CV, AveVal};

        %% New Feature Generation
        % L, hMin, hMax, nMin, nMax, HNR

        centroid=sum(v)/size(v,1);
        t_v=v-centroid;
        t_basePoint=basePoint-centroid;

        axis=cross(t_basePoint,[0,-1,0]);
        axis=axis/norm(axis);
        a=norm(t_basePoint-[0,-1,0]);
        b=norm(t_basePoint);
        c=1;
        rotationVector=acos((b*b+1-a*a)/(2*b)) * axis;
        rotationMatrix=rotationVectorToMatrix(rotationVector);
        t_v=t_v(:,1:3)*rotationMatrix;
        t_basePoint=t_basePoint*rotationMatrix;
        lmaxY=max(t_v(:,2));
        lminY=min(t_v(:,2));
        if lminY<t_basePoint(2)
            lminY=t_basePoint(2);
        end
        stepSize=Spinelength/10;
        if Spinelength<0.5
            stepSize=0.1;
        end

        stepNum=(lmaxY-lminY)/stepSize+1;
        local_max_d=0;
        local_min_d=0;
        local_angle=0;
        local_center=0;
        neck_end_step=1;
        for i=1:stepNum
            localV=t_v(t_v(:,2)<stepSize*i+lminY & t_v(:,2)>=stepSize*(i-1)+lminY,:);
            index=find(t_v(:,2)<stepSize*i+lminY & t_v(:,2)>=stepSize*(i-1)+lminY);
            dmax=0;
            lmaxv1=0;
            lmaxv2=0;
            for j=1:size(localV,1)
                for jj=j+1:size(localV,1)
                    td=norm(localV(j,[1 3])-localV(jj,[1 3]));
                    if td>dmax
                        dmax=td;
                        lmaxv1=j;
                        lmaxv2=jj;
                    end
                end
            end

            local_max_d(i)=0;
            local_min_d(i)=0;
            local_angle(i)=0;
            local_center(i,1:3)=zeros(1,3);
            if size(localV,1)>1
                axis=cross([localV(lmaxv1,1)-localV(lmaxv2,1),0,localV(lmaxv1,3)-localV(lmaxv2,3)],[1,0,0]);
                axis=axis/norm(axis);
                a=norm(localV(lmaxv1,[1 3])-localV(lmaxv2,[1 3])-[1,0]);
                b=norm(localV(lmaxv1,[1 3])-localV(lmaxv2,[1 3]));
                c=1;
                rotationVector=acos((b*b+1-a*a)/(2*b)) * axis;
                rotationMatrix=rotationVectorToMatrix(rotationVector);
                t_localV=localV(:,1:3)*rotationMatrix;

                lminv1=find(t_localV(:,3)==min(t_localV(:,3)),1);
                lminv2=find(t_localV(:,3)==max(t_localV(:,3)),1);

                local_max_d(i)=dmax;
                local_min_d(i)=max(t_localV(:,3))-min(t_localV(:,3));
                local_angle(i)=acos((b*b+1-a*a)/(2*b));
                local_center(i,1:3)=(v(index(lmaxv1),:)+v(index(lmaxv2),:)+v(index(lminv1),:)+v(index(lminv2),:))*0.25;
                %local_center(i,1:3)=(t_localV(lmaxv1,:)+t_localV(lmaxv2,:)+t_localV(lminv1,:)+t_localV(lminv2,:))*0.25;

            else
                if i==1
                    local_center(i,1:3)=basePoint;
                else
                    local_center(i,1:3)=local_center(i-1,1:3);
                end
            end

            if stepSize*i+lminY<0
                neck_end_step=i;
            end
        end

        local_center(1:floor(stepNum),:)=[smooth(local_center(1:floor(stepNum),1)),smooth(local_center(1:floor(stepNum),2)),smooth(local_center(1:floor(stepNum),3))];

        skeletonCSV=fopen([resultPath,'spine/',num2str(k),'.skeleton'],'w');
        %Format: 
        %stepnum
        %neck_end_step max_head_step min_neck_step 
        %x y z local_max_d local_min_d angle
        %...

        max_head_step=find(local_max_d(:)==max(local_max_d(neck_end_step+1:end)),1);
        min_neck_step=find(local_max_d(:)==min(local_max_d(1:neck_end_step)),1);


        fprintf(skeletonCSV,'%f\n',stepNum);
        fprintf(skeletonCSV,'%f %f %f\n',neck_end_step,max_head_step,min_neck_step);

        len=0;
        for i=1:stepNum
            fprintf(skeletonCSV,'%f %f %f %f %f %f\n',local_center(i,1),local_center(i,2),local_center(i,3),local_max_d(i),local_min_d(i),local_angle(i));
            if i>1
                len=len+norm(local_center(i,:)-local_center(i-1,:));
            end
        end
        fclose(skeletonCSV);

        Features(k,6:13)={len,local_min_d(max_head_step),local_max_d(max_head_step),local_min_d(min_neck_step),local_max_d(min_neck_step),local_max_d(max_head_step)/local_max_d(min_neck_step),0,'None'};
        Correctness(k)=1;
    end


    labelCSV=fopen([resultPath,'spineLabel.csv'],'w');
    fprintf(labelCSV,'Spine Name,Label\n');
    for k=1:length(Correctness)
        if Correctness(k)==1
            fprintf(labelCSV,'%s,%d\n',num2str(k),k);
        end
    end
    fclose(labelCSV);

    infoCSV=fopen([resultPath,'spine/spineInfo.csv'],'w');
    fprintf(infoCSV,'Spine Name,V,CHR,CVD,OA,L,hMin,hMax,nMin,nMax,HNR,SA,Type\n');
    for k=1:length(Correctness)
        if Correctness(k)==1
            t=cell2mat(Features(k,2:12));
            fprintf(infoCSV,'%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%s\n',char(Features(k,1)),t(1),t(2),t(3),t(4),t(5),t(6),t(7),t(8),t(9),t(10),t(11),char(Features(k,13)));
        end
    end
    fclose(infoCSV);
    
    datasetInfo=fopen([resultPath,'dataset.DXplorerInfo'],'w');
    fprintf(datasetInfo,'Input Image: %s\n',inputPath);
    fprintf(datasetInfo,'Original Path: %s\n',resultPath);
    fclose(datasetInfo);

end



