varying vec3 vert;

void main() {
   vert = gl_Vertex.xyz;
   gl_Position = gl_Vertex;
}
