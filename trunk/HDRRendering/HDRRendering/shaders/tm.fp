uniform sampler2D fullTex;
uniform sampler2D bluredTex;
uniform float exposureLevel;

float lerp( float v1,float v2,float lerp )
{
    return v1 + lerp * (v2 - v1);
}

vec4 lerp( vec4 v1, vec4 v2, float lerp )
{
	return vec4( lerp( v1.x, v2.x, lerp ),
		lerp( v1.y, v2.y, lerp ),
		lerp( v1.z, v2.z, lerp ),
		lerp( v1.w, v2.w, lerp ) );
}

vec4 pow( vec4 v, float p )
{
	return vec4( pow( v.x, p ),
		pow( v.y, p ),
		pow( v.z, p ),
		pow( v.w, p ) );
}

void main( void )
{
	vec4 original = texture2D( fullTex, gl_TexCoord[0].st );
	vec4 blur = texture2D( bluredTex, gl_TexCoord[0].st );
	
	vec4 color = lerp( original, blur, 0.4f );
	vec2 coord = gl_TexCoord[0].st - 0.5;
	float vignette = 1 - dot( coord, coord );
	color *= pow( vignette, 4.0 );
	color *= exposureLevel;
	
	gl_FragColor = pow( color, 0.55 );
}