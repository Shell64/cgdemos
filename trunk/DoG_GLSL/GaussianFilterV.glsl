const int MaxKernelSize = 100;

uniform sampler2DRect Tex;

uniform float Kernel[ MaxKernelSize ];

uniform int KernelSize;

uniform float Offset[ MaxKernelSize ];

void main( void )
{
	float intensity = 0.0;
	vec2 pos, original;	
	int count = 0;
	
	for ( int i = 0; i < KernelSize; ++i )
	{
		if ( Offset[ i ] == 0.0 )
		{
			original = texture2DRect( Tex, gl_TexCoord[0].st).rb;
			intensity += original.y * Kernel[ i ];
			++count;
		}
		else
		{
			pos = gl_TexCoord[0].st + vec2( 0, Offset[ i ] );			
			intensity += texture2DRect( Tex, pos ).b * Kernel[ i ];
		}
	}
	
	gl_FragColor.r = intensity;
	gl_FragColor.g = intensity - original.x;
	gl_FragColor.b = original.y;
}