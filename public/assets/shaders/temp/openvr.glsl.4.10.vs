#version 410 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord0;
layout(location = 2) in vec2 inTexCoord1;
layout(location = 3) in vec2 inTexCoord2;

noperspective out vec2 outTexCoord0;
noperspective out vec2 outTexCoord1;
noperspective out vec2 outTexCoord2;

void main()
{
gl_Position  = inPosition;
outTexCoord0 = inTexCoord0; // Red
outTexCoord1 = inTexCoord1; // Green
outTexCoord2 = inTexCoord2; // Blue
}