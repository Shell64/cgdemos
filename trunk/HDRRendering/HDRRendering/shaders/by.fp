uniform sampler2D Tex;
uniform float step;

const int k_size = 4;
uniform float weight[k_size];

void main()
{
	vec4 color = texture2D( Tex, gl_TexCoord[0].st ) * weight[0];
	vec2 offset = vec2( 0, step );
	for ( int i = 1; i < k_size; ++i )
	{
		color += texture2D( Tex, gl_TexCoord[0].st + ( offset * i ) ) * weight[i];
		color += texture2D( Tex, gl_TexCoord[0].st - ( offset * i ) ) * weight[i];
	}
	//color = vec4(0.f, 1.f, 0.f, 1.f );
	gl_FragColor = color;
}