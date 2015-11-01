uniform sampler2D inTexture;

noperspective in vec2 outTexCoord0; // Red
noperspective in vec2 outTexCoord1; // Green
noperspective in vec2 outTexCoord2; // Blue

out vec4 outputColor;

void main()
{
float fBoundsCheck = ( (dot( vec2( lessThan( outTexCoord1.xy, vec2(0.05, 0.05)) ), vec2(1.0, 1.0))+dot( vec2( greaterThan( outTexCoord1.xy, vec2( 0.95, 0.95)) ), vec2(1.0, 1.0))) );

if ( fBoundsCheck > 1.0 )
   { 
   outputColor = vec4( 0, 0, 0, 1.0 ); 
   }
else
   {
   float red   = texture(inTexture, outTexCoord0).r;
   float green = texture(inTexture, outTexCoord1).g;
   float blue  = texture(inTexture, outTexCoord2).b;
   outputColor = vec4(red, green, blue, 1.0); 
   }
}