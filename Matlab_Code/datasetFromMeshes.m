f1='C:\Users\Jun2\Desktop\SpineAnalysis\FeatureComparison\M_13.ply';
f2='C:\Users\Jun2\Desktop\SpineAnalysis\FeatureComparison\mush24.ply';

inputPath='ttt';
resultPath='D:\featureCompare\';

    smoothingItt=5;

    for k = 1:2 
        k
        %Calculate mesh from isolated spine candidates    
        if k==1
            [v,tri]=read_ply(f1);
        else
            [v,tri]=read_ply(f2);
        end
        for i=1:length(tri)
            t=tri(i,1);
            tri(i,1)=tri(i,3);
            tri(i,3)=t;
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
        
        tcurv=rmoutliers(GausianCurvature(GausianCurvature<0),'percentiles',[1 100]);
        tmin=min(tcurv(:));
        GausianCurvature(GausianCurvature<tmin)=tmin;
        
        tcurv=rmoutliers(GausianCurvature(GausianCurvature>0),'percentiles',[0 99]);
        tmax=max(tcurv(:));
        GausianCurvature(GausianCurvature>tmax)=tmax;

        tcurv=rmoutliers(MeanCurvature(MeanCurvature<0),'percentiles',[1 100]);
        tmin=min(tcurv(:));
        MeanCurvature(MeanCurvature<tmin)=tmin;
        
        tcurv=rmoutliers(MeanCurvature(MeanCurvature>0),'percentiles',[0 99]);
        tmax=max(tcurv(:));
        MeanCurvature(MeanCurvature>tmax)=tmax;

        
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




