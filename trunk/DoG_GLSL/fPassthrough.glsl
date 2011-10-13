//fPassthrough.glsl

//Pass through fragment shader

uniform float time;

void main()
{
	vec4 color = gl_Color;
	int t = time * 0.001;
	if ( ( t % 3 ) == 0 )
		color.r = 100;
	else if ( ( t % 3 ) == 1 )
		color.g = 100;
	else if ( ( t % 3 ) == 2 )
		color.b = 100;
	gl_FragColor = color;
}