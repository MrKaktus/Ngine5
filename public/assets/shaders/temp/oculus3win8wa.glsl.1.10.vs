#version 110

attribute vec2  inPosition;
attribute vec2  inTexCoord0;
attribute vec2  inTexCoord1;
attribute vec2  inTexCoord2;
attribute float inVignette;
attribute float inTimeWarp;

varying vec4  Position;
varying float Vignette;
varying vec2  TexCoord0;
varying vec2  TexCoord1;
varying vec2  TexCoord2;

uniform vec2 EyeToSourceUVScale;
uniform vec2 EyeToSourceUVOffset;
uniform mat4 EyeRotationStart;
uniform mat4 EyeRotationEnd;

vec2 TimewarpTexCoord(vec2 TexCoord, mat4 rotMat)
{
// Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic
// aberration and distortion). These are now "real world" vectors in direction (x,y,1)
// relative to the eye of the HMD. Apply the 3x3 timewarp rotation to these vectors.
vec3 transformed = vec3( vec4(rotMat * vec4(TexCoord.xy, 1.0, 1.0) ).xyz);

// Project them back onto the Z=1 plane of the rendered images.
vec2 flattened = (transformed.xy / transformed.z);

// Scale them into ([0,0.5],[0,1]) or ([0.5,0],[0,1]) UV lookup space (depending on eye)
return (EyeToSourceUVScale * flattened + EyeToSourceUVOffset);
}

void main()
{	
mat4 lerpedEyeRot = ((1.0 - inTimeWarp) * EyeRotationStart) + 
                            (inTimeWarp * EyeRotationEnd);

Position  = vec4(inPosition.xy, 0.5, 1.0);
Vignette  = inVignette;                             
TexCoord0 = TimewarpTexCoord(inTexCoord0, lerpedEyeRot);
TexCoord1 = TimewarpTexCoord(inTexCoord1, lerpedEyeRot);
TexCoord2 = TimewarpTexCoord(inTexCoord2, lerpedEyeRot);

// Windows 8.1 flip fix WA
Position.x *= -1.0;
Position.y *= -1.0;

// To be compatible with GLSL 1.10
gl_Position = Position;
}