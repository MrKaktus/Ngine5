#version 110

uniform sampler2D enDiffuse;

varying vec2 texcoord0;

void main()
{
	gl_FragColor = texture2D(enDiffuse, texcoord0);
}
