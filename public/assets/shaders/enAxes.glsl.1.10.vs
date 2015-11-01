uniform mat4 enModelViewProjection;

attribute vec3 inPosition;
attribute vec3 inColor;

varying vec3 Color;

void main()
{	
gl_Position = enModelViewProjection * vec4(inPosition, 1.0); 
Color       = inColor;
}
