uniform sampler2DRect Tex;

void main()
{
	gl_FragColor.r = gl_FragColor.g = gl_FragColor.b = gl_FragColor.a = texture2DRect( Tex, gl_TexCoord[0].st).g;
}