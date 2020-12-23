#version 140
#extension GL_ARB_compatibility: enable

attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 curvature;

varying vec3 vert;
varying vec3 vertNormal;
varying vec2 curv;
varying vec3 localLightPos;

uniform mat4 projMatrix;
uniform mat4 mvMatrix;
uniform mat4 mvMatrix2;
uniform mat3 normalMatrix;
uniform int mode;
uniform vec3 lightPos;

void main() {
   curv=curvature;
   vert = vec3(projMatrix * mvMatrix * vertex);
   vertNormal = normalize(vec3(projMatrix * mvMatrix * vec4(normal,1)));
   localLightPos = lightPos;//vec3(mvMatrix * vec4(lightPos,1));
   if(mode==1)gl_Position = projMatrix * mvMatrix * vertex;
   else if(mode==3){
       gl_Position = projMatrix * mvMatrix2 * vertex;
       gl_Position.x=-vertex.x;
   }
   else if(mode==5){
       gl_Position = projMatrix * mvMatrix * (vertex + vec4(normal*0.002,0));
   }
   else if(mode==4){
       gl_Position = projMatrix * mvMatrix * (vertex + vec4(normal*0.0015,0));
   }
   else if(mode==2){
       gl_Position =  projMatrix * mvMatrix *vertex;
   }
   else gl_Position = projMatrix * mvMatrix * (vertex + vec4(normal*0.001,0));
}
