uniform sampler2D Tex;

vec4 SuppressLDR( vec4 c )
{
	if( c.r > 1.0f || c.g > 1.0f || c.b > 1.0f )
		return c;
	else
		return vec4( 0.0f, 0.0f, 0.0f, 0.0f );
}

void main()
{
	vec4 color = texture2D( Tex, gl_TexCoord[0].st ) * 1.0001f;
	
	gl_FragColor = SuppressLDR( color );
}