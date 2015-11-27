uniform mat4 enModelMatrix;

in vec3 inPosition;
in vec3 inColor;

out vec3 Color;

void main()
{	
gl_Position = enOutVertexPosition(enCamViewProjection() * enModelMatrix * vec4(inPosition, 1.0) ); 
Color       = inColor;
}
