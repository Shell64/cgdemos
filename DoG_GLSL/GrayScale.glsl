uniform sampler2DRect Tex;

void main()
{
	float intensity = dot( vec3( 0.299, 0.587, 0.114 ),
		texture2DRect( Tex, gl_TexCoord[0].st ).rgb );
	gl_FragColor = vec4( intensity, intensity, intensity, 1.0 );
}