#version 120

varying vec2 vTexCoord;
varying vec3 vVertex;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vTexCoord = gl_MultiTexCoord0.st;
    vVertex = gl_Vertex.xyz;
}
