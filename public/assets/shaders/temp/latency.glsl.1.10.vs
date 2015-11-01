#version 110

uniform mat4 enProjection;

attribute vec4 inPosition;

void main()
{	
gl_Position = enProjection * inPosition; 
}