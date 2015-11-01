#version 110

uniform sampler2D inTexture;

varying vec4  Position;
varying vec2  TexCoord0;
varying vec2  TexCoord1;
varying vec2  TexCoord2;
varying float Vignette;

void main()
{
// Compose final color
gl_FragColor = vec4( texture2D(inTexture, TexCoord0.xy).r * Vignette,
                     texture2D(inTexture, TexCoord1.xy).g * Vignette,
                     texture2D(inTexture, TexCoord2.xy).b * Vignette,
                     1.0 );
}
