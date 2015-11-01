#version 110

uniform mat4 enModelViewProjection;

attribute vec3 inPosition;
attribute vec2 inTexCoord0;

varying vec2 texcoord0;

void main()
{	
	texcoord0 = inTexCoord0;
	gl_Position = enModelViewProjection * vec4(inPosition, 1.0);
}
