
const int MaxKernelSize = 100;

uniform sampler2DRect Tex;

uniform float Kernel[ MaxKernelSize ];

uniform int KernelSize;

uniform float Offset[ MaxKernelSize ];

void main( void )
{
	float intensity = 0.0, original = 0.0;
	vec2 pos;		
	
	for ( int i = 0; i < KernelSize; ++i )
	{
		if ( Offset[ i ] == 0.0 )
		{
			original = texture2DRect( Tex, gl_TexCoord[0].st).r;
			intensity += original * Kernel[ i ];			
		}
		else
		{
			pos = gl_TexCoord[0].st + vec2( Offset[ i ], 0 );			
			intensity += texture2DRect( Tex, pos ).r * Kernel[ i ];
		}
	}
	
	gl_FragColor.r = original;
	gl_FragColor.b = intensity;	
}