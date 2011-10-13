uniform sampler2DRect Tex;

void main()
{
	gl_FragColor = texture2DRect( Tex, gl_TexCoord[0].st).g;
}