// Color of the material
uniform vec4 matColor;

// Cube map
uniform samplerCube env;

// Reflection factor
uniform float reflectionFactor;

// Reflected vector
varying vec3 R;

void main()
{
	vec4 color = mix(matColor, textureCube(env, R), reflectionFactor);
	color.a = 1.0;
	gl_FragColor = color;
}