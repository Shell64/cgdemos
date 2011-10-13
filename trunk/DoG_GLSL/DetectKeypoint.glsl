#define THRESHOLD 0.0

uniform float contr_thr;
uniform float edge_threshold;

uniform sampler2DRect Tex, TexU, TexD;

void main()
{
	//vector d1 is < (-1, 0), (1, 0), (0,-1), (0, 1)>
	//vector d2 is < (-1,-1), (-1,1), (1,-1), (1, 1)>
	vec4 d1, d2, gg, temp;
	vec2 CoordRU = vec2( gl_TexCoord[2].x, gl_TexCoord[4].y );
	vec4 current = texture2DRect( Tex, gl_TexCoord[0].xy );

	temp = texture2DRect( Tex, gl_TexCoord[1].xy );
	d1.x = temp.g;	gg.x = temp.r;
	temp = texture2DRect( Tex, gl_TexCoord[2].xy );
	d1.y = temp.g;	gg.y = temp.r;
	temp = texture2DRect( Tex, gl_TexCoord[3].xy );
	d1.z = temp.g;	gg.z = temp.r;
	temp = texture2DRect( Tex, gl_TexCoord[4].xy );
	d1.w = temp.g;	gg.w = temp.r;

	d2.x = texture2DRect( Tex, gl_TexCoord[5].xy ).g;
	d2.y = texture2DRect( Tex, gl_TexCoord[6].xy ).g;
	d2.z = texture2DRect( Tex, gl_TexCoord[7].xy ).g;
	d2.w = texture2DRect( Tex, CoordRU.xy ).g;

	//compute gradient magnitude and orientation
	vec2 dxdy = gg.yw - gg.xz; //x,y--left and right
	float grad = 0.5 * length( dxdy ); //I'm not sure why it multiplies 0.5 here
	float theta = grad == 0 ? 0.0 : atan( dxdy.y, dxdy.x );
	gl_FragData[0] = vec4( current.rg, grad, theta );

	//use variable to identify type of extremum
	//1.0 for local maximum and 0.5 for minimum
	float is_key = 0.0;
	gl_FragData[1] = vec4( is_key, 0.0, 0.0, 0.0 );
	is_key = current.g > THRESHOLD && 
		all( greaterThan( current.gggg, max( d1, d2 ) ) ) ?
		1.0 : 0.0;
	is_key = current.g < THRESHOLD &&
		all( lessThan( current.gggg, min( d1, d2 ) ) ) ?
		0.5 : 0.0;
	if ( 0.0 == is_key ) return;

	float fxx, fyy, fxy;
	vec4 D2 = d1.xyzw - current.gggg;
	vec2 D4 = d2.xw - d2.yz;
	fxx = D2.x + D2.y;
	fyy = D2.z + D2.w;
	fxy = 0.25 * ( D4.x + D4.y );

	// eliminating edge responses, refer to Lowe's section 4.1
	float fxx_plus_fyy = fxx + fyy;
	float alpha_beta = fxx*fyy - fxy*fxy;
	float squa = fxx_plus_fyy * fxx_plus_fyy;
	if ( alpha_beta < 0 || squa >= edge_threshold * alpha_beta ) return;

	vec2 D5 = 0.5 * ( d1.yw - d1.xz );
	float fx = D5.x;
	float fy = D5.y;
	float fs, fss, fxs, fys;
	vec2 d3; 
	vec4 d4, d5, d6;
	d3.x = texture2DRect( TexU, gl_TexCoord[0].xy ).g;
	d4.x = texture2DRect( TexU, gl_TexCoord[1].xy ).g;
	d4.y = texture2DRect( TexU, gl_TexCoord[2].xy ).g;
	d4.z = texture2DRect( TexU, gl_TexCoord[3].xy ).g;
	d4.w = texture2DRect( TexU, gl_TexCoord[4].xy ).g;
	d6.x = texture2DRect( TexU, gl_TexCoord[5].xy ).g;
	d6.y = texture2DRect( TexU, gl_TexCoord[6].xy ).g;
	d6.z = texture2DRect( TexU, gl_TexCoord[7].xy ).g;
	d6.w = texture2DRect( TexU, CoordRU.xy ).g;

	if ( is_key == 1.0 )
	{
		if ( current.g < d3.x ||
			any( lessThan( current.gggg, d4 ) ||
			any( lessThan( current.gggg, d6 ) )
			return;
		d3.x = texture2DRect( TexD, gl_TexCoord[0].xy ).g;
		d5.x = texture2DRect( TexD, gl_TexCoord[1].xy ).g;
		d5.y = texture2DRect( TexD, gl_TexCoord[2].xy ).g;
		d5.z = texture2DRect( TexD, gl_TexCoord[3].xy ).g;
		d5.w = texture2DRect( TexD, gl_TexCoord[4].xy ).g;
		d6.x = texture2DRect( TexD, gl_TexCoord[5].xy ).g;
		d6.y = texture2DRect( TexD, gl_TexCoord[6].xy ).g;
		d6.z = texture2DRect( TexD, gl_TexCoord[7].xy ).g;
		d6.w = texture2DRect( TexD, CoordRU.xy ).g;
		if ( current.g < d3.y ||
			any( lessThan( current.gggg, d5 ) ||
			any( lessThan( current.gggg, d6 ) )
			return;
	}
	else
	{
		if ( current.g > d3.x ||
			any( greaterThan( current.gggg, d4 ) ||
			any( greaterThan( current.gggg, d6 ) )
			return;
		d3.x = texture2DRect( TexD, gl_TexCoord[0].xy ).g;
		d5.x = texture2DRect( TexD, gl_TexCoord[1].xy ).g;
		d5.y = texture2DRect( TexD, gl_TexCoord[2].xy ).g;
		d5.z = texture2DRect( TexD, gl_TexCoord[3].xy ).g;
		d5.w = texture2DRect( TexD, gl_TexCoord[4].xy ).g;
		d6.x = texture2DRect( TexD, gl_TexCoord[5].xy ).g;
		d6.y = texture2DRect( TexD, gl_TexCoord[6].xy ).g;
		d6.z = texture2DRect( TexD, gl_TexCoord[7].xy ).g;
		d6.w = texture2DRect( TexD, CoordRU.xy ).g;
		if ( current.g > d3.y ||
			any( greaterThan( current.gggg, d5 ) ||
			any( greaterThan( current.gggg, d6 ) )
			return;
	}

	fs = 0.5 * ( d3.x - d3.y );
	fss = d3.x + d3.y - current.g - current.g;
	fxs = 0.25 * ( d4.y + d5.x - d4.x - d5.y );
	fys = 0.25 * ( d4.w + d5.z - d4.z - d5.w );
	
	//that is 
	// |-fx|     | fxx fxy fxs |   |dx|
	// |-fy|  =  | fxy fyy fys | * |dy|
	// |-fs|     | fxs fys fss |   |ds|
	// need to solve dx, dy, ds

	// Use Gauss elimination to solve the linear system
	vec3 dxys = vec3( 0.0 );	
	vec4 A0 = vec4( fxx, fxy, fxs, -fx );
	vec4 A1 = vec4( fxy, fyy, fys, -fy );
	vec4 A2 = vec4( fxs, fys, fss, -fs );

	vec3 x3 = abs( vec3( fxx, fxy, fxs ) );
	float maxma = max( max( x3.x, x3.y ), x3.z );
	if ( maxma < 1e-10 ) return;
	if ( x3.y == maxma ) {
		vec4 TEMP = A1; A1 = A0; A0 = TEMP;
	}
	else if ( x3.z == maxma ) {
		vec4 TEMP = A2; A2 = A0; A0 = TEMP;
	}

	A0 /= A0.x;
	A1 -= A1.x * A0;
	A2 -= A2.x * A0;
	vec2 x2 = abs( vec2( A1.y, A2.y ) );
	if ( x2.y > x2.x ) {
		vec3 TEMP = A2.yzw;
		A2.yzw = A1.yzw;
		A1.yzw = TEMP;
		x2.x = x2.y;
	}
	if ( x2.x < 1e-10 ) return;

	A1.yzw /= A1.y;
	A2.yzw -= A2.y * A1.yzw;
	if ( abs( A2.z ) < 1e-10 ) return;

	dxys.z = A2.w / A2.z;
	dxys.y = A1.w - A1.z * dxys.z;
	dxys.s = A0.w - A0.z * dxys.z - A0.y * dxys.y;

	bool contr_test = ( abs( current.g + 
		0.5 * dot( vec3( fx, fy, fs ), dxys ) ) <= contr_thr );
	if ( contr_test || any( greaterThan( abs( dxys, vec3( 1.0 ) ) ) )
		return;
	gl_FragData[1] = vec4( is_key, dxys );
}