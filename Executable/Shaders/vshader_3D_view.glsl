#version 140
#extension GL_ARB_compatibility: enable
out vec3 pixel_position;

void main(){
    pixel_position=vec3(gl_Vertex);
    gl_Position =gl_Vertex;
}