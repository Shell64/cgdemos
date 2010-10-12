void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	    // pass through texcoord
    gl_TexCoord[0].st = gl_MultiTexCoord0.st;
}