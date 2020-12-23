#version 140
#extension GL_ARB_compatibility: enable

varying vec3 vert;
varying vec3 vertNormal;
varying vec2 curv;
varying vec3 localLightPos;
uniform int mode;
uniform float curvMin;
uniform float curvMax;
uniform bool curvType;
void main() {
   if(mode==1 || mode==0){

       vec3 L = normalize(localLightPos - vert);
       vec3 E = normalize(vec3(0,0,-100)-vert); // we are in Eye Coordinates, so EyePos is (0,0,0)
       vec3 R = normalize(-reflect(L,vertNormal));

       vec4 Idiff = vec4(0.5,0.5,0.5,0.5) * max(dot(vertNormal,L), 0.0);
       Idiff = clamp(Idiff, 0.0, 1.0);

       vec3 color = vec3(0.8,0.8,0.8);
       float curvValue;
       vec2 tCurv=vec2(curv.x,curv.y);
       if(curvType){
           if(tCurv.y>curvMax)curvValue=curvMax;
           else if(tCurv.y<curvMin)curvValue=curvMin;
           else curvValue=tCurv.y;
           if(tCurv.y==-2){
               curvValue=-2;
           }
           else if(tCurv.y<0){
               curvValue=tCurv.y;
           }

       }
       else{
           if(tCurv.x>curvMax)curvValue=curvMax;
           else if(tCurv.x<curvMin)curvValue=curvMin;
           else curvValue=tCurv.x;
           if(tCurv.x==-2){
               curvValue=-2;
           }
           else if(tCurv.x<0){
               curvValue=tCurv.x;
           }

       }
       if(curvValue==-2){
           color=vec3(1,1,1);
       }
       else if(curvValue<0){
           curvValue+=1;
           float cV=(curvValue-curvMin)/(curvMax-curvMin);
           if(cV<0.5){
               color =  vec3(0.2,0.7,0.6) * cV * 2 + vec3(0,0,0.5) * (1.0-cV*2);
           }
           else{
               color =  vec3(1.0,1.0,0) * (cV-0.5)*2 + vec3(0.2,0.7,0.6) * (1.0-(cV-0.5)*2);
           }
           color=color*0.3+vec3(1.0,0,0)*0.7;
       }
       else{
           float cV=(curvValue-curvMin)/(curvMax-curvMin);
           if(cV<0.5){
               color =  vec3(0.2,0.7,0.6) * cV * 2 + vec3(0,0,0.5) * (1.0-cV*2);
           }
           else{
               color =  vec3(1.0,1.0,0) * (cV-0.5)*2 + vec3(0.2,0.7,0.6) * (1.0-(cV-0.5)*2);
           }
       }

       gl_FragColor = vec4(color,1.0f)*1.15 + Idiff*0.5;
   }
   else if(mode==3){
       gl_FragColor=vec4(1.0,0.5,0.5,1.0);
   }
   else if(mode==4){
       gl_FragColor=vec4(0,8.0/255,115.0/255,1.0);
   }
   else if(mode==5){

       vec3 L = normalize(localLightPos - vert);
       vec3 E = normalize(vec3(0,0,-100)-vert); // we are in Eye Coordinates, so EyePos is (0,0,0)
       vec3 R = normalize(-reflect(L,vertNormal));

       vec4 Idiff = vec4(0.5,0.5,0.5,0.5) * max(dot(vertNormal,L), 0.0);
       Idiff = clamp(Idiff, 0.0, 1.0);

       vec3 color = vec3(0.8,0.8,0.8);
       float curvValue;
       vec2 tCurv=vec2(curv.x,curv.y);
       if(curvType){
           if(tCurv.y>curvMax)curvValue=curvMax;
           else if(tCurv.y<curvMin)curvValue=curvMin;
           else curvValue=tCurv.y;
           if(tCurv.y==-2){
               curvValue=-2;
           }
           else if(tCurv.y<0){
               curvValue=tCurv.y;
           }

       }
       else{
           if(tCurv.x>curvMax)curvValue=curvMax;
           else if(tCurv.x<curvMin)curvValue=curvMin;
           else curvValue=tCurv.x;
           if(tCurv.x==-2){
               curvValue=-2;
           }
           else if(tCurv.x<0){
               curvValue=tCurv.x;
           }

       }
       if(curvValue<0){
           curvValue+=1;
           float cV=(curvValue-curvMin)/(curvMax-curvMin);
           if(cV<0.5){
               color =  vec3(0.2,0.7,0.6) * cV * 2 + vec3(0,0,0.5) * (1.0-cV*2);
           }
           else{
               color =  vec3(1.0,1.0,0) * (cV-0.5)*2 + vec3(0.2,0.7,0.6) * (1.0-(cV-0.5)*2);
           }
           color=color*0.3+vec3(1.0,0,0)*0.7;
           gl_FragColor = vec4(color,1.0f)*1.15 + Idiff;
       }
       else{
           discard;
       }
   }
   else if(mode==2){
       gl_FragColor=vec4(0,0,0,1);
   }
   if(mode==0){
        gl_FragColor=gl_FragColor*0.9;
   }
}
