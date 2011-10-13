// GLSLDemo.cpp : Defines the entry point for the console application.
//

/* GLSL demo program*/



#define GL_GLEXT_PROTOTYPES
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <ctime>

#include "framebufferObject.h"
#include "il/il.h"
#include "glErrorUtil.h"
#include "ShaderObject.h"
#include "ProgramGLSL.h"

const GLdouble nearVal = 1.0; /*near distance*/
const GLdouble farVal = 20.0; /*far distance*/

char* ebuffer; /*buffer for error messages*/
GLint elength; /*length of error messages*/

FramebufferObject* g_FBO;
GLuint g_MonoTex;
GLuint g_ColorTex;

ProgramGLSL* g_hProgram;
ProgramGLSL* g_vProgram;
ProgramGLSL* g_sProgram;

int g_img_width = 0;
int g_img_height = 0;

unsigned char* CreateGrayImage(
              const unsigned char* img, 
              int width, int height, int format ) {
  if ( NULL == img || width == 0 || height == 0 ) return NULL;

  unsigned char* gray_image = new unsigned char[ width * height ];	
  if ( NULL == gray_image )
    return NULL;

  const unsigned char *line = img, *p;
  unsigned char* po = gray_image;
  int step = format == GL_RGB? 3: 4 ; 
  int linestep = width * step; 

  for( int i = 0, j = 0 ; i < height; ++i, line += linestep )
  {
    for( j = 0, p = line; j < width; ++j, p += step )
    {
      //*po++ = int(p[0]*0.299 + p[1] * 0.587 + p[2]* 0.114 + 0.5);
      *po++ = ( ( 19595*p[0] + 38470*p[1] + 7471*p[2]+ 32768 ) >> 16 );
    }
  }
  return gray_image;
}

/*standard OpenGL initialization*/
static void initialize(const char *filename)
{	
  ilInit();
  ilOriginFunc( IL_ORIGIN_UPPER_LEFT );
  ilEnable( IL_ORIGIN_SET );

  unsigned int imId = ilGenImage();
  ilBindImage( imId );
  ilLoadImage(filename);
  g_img_width = ilGetInteger( IL_IMAGE_WIDTH );
  g_img_height = ilGetInteger( IL_IMAGE_HEIGHT );
  //glutReshapeWindow( g_img_width, g_img_height );

  int ilformat = ilGetInteger( IL_IMAGE_FORMAT );	
  const unsigned char* color_image = (const unsigned char*)ilGetData();
  unsigned char* gray_image = CreateGrayImage( 
    color_image, g_img_width, g_img_height, ilformat );
  ilDeleteImage( imId );

  glPixelStorei(GL_UNPACK_ALIGNMENT , 1); 

  glGenTextures( 1, &g_MonoTex );
  glBindTexture( GL_TEXTURE_RECTANGLE_ARB, g_MonoTex );
  glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); 
  glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); 
  glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); 
  glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

  glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, 
    GL_LUMINANCE32F_ARB, g_img_width, g_img_height, 0, 
    GL_LUMINANCE, GL_UNSIGNED_BYTE, gray_image );

  delete[] gray_image;

  glGenTextures( 1, &g_ColorTex );
  glBindTexture( GL_TEXTURE_RECTANGLE_ARB, g_ColorTex );
  glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); 
  glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); 
  glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); 
  glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, g_img_width, g_img_height,
    0, GL_RGB, GL_FLOAT, 0);

  g_FBO = new FramebufferObject();
  g_FBO->Bind();	
  g_FBO->AttachTexture( GL_TEXTURE_RECTANGLE_ARB, g_ColorTex, GL_COLOR_ATTACHMENT0_EXT );
  g_FBO->IsValid();

  FramebufferObject::Disable();
}

float* g_kernel;
float* g_offset;
int g_kernel_size = 0;

void calculate_kernel( float sigma )
{
  //pixel inside 3*sigma box
  int sz = int( ceil( 4 * sigma - 0.5) ) ;//
  g_kernel_size = 2 * sz + 1;	

  g_kernel = new float[ g_kernel_size ];
  g_offset = new float[ g_kernel_size ];
  float rv = 1.0f / ( sigma * sigma );
  float v, ksum =0; 

  // pre-compute filter
  for ( int i = -sz ; i <= sz ; ++i ) 
  {
    g_kernel[ i + sz ] = v = exp( -0.5f * i * i * rv ) ;
    ksum += v;
    g_offset[ i + sz ] = float( i );
  }

  //normalize the kernel
  rv = 1.0f / ksum;
  for ( int i = 0; i< g_kernel_size; ++i )
    g_kernel[ i ] *= rv;
}

float g_sigma = 3.0;

void one_pass( ProgramGLSL& program, float width )
{
  GLint kernelParam = program.GetUniformLocation( "Kernel" ) ;
  GLint offsetParam = program.GetUniformLocation( "Offset" );
  GLint kernelSizeParam = program.GetUniformLocation( "KernelSize" );
  GLint texParam = program.GetUniformLocation( "Tex" );

  float *offset = new float[ g_kernel_size ];
  for ( int  i = 0; i < g_kernel_size; ++i )
  {
    offset[ i ] = g_offset[ i ];
  }

  program.UseProgram();
  glUniform1fv( kernelParam, g_kernel_size, g_kernel );
  glUniform1fv( offsetParam, g_kernel_size, offset );
  glUniform1i( kernelSizeParam, g_kernel_size );
  glUniform1i( texParam, 0 );

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0, g_img_height); glVertex2f(0, g_img_height );
    glTexCoord2f(g_img_width, g_img_height); glVertex2f( g_img_width, g_img_height);
    glTexCoord2f(g_img_width, 0); glVertex2f( g_img_width, 0 );
    glTexCoord2f(0, 0); glVertex2f(0, 0 );
  }
  glEnd();
}

void filter()
{	
  //glEnable( GL_TEXTURE_RECTANGLE_ARB );
  calculate_kernel( g_sigma );

  GLint currentDrawBuf;
  glGetIntegerv( GL_DRAW_BUFFER, &currentDrawBuf );

  int vp[4];
  glGetIntegerv( GL_VIEWPORT, vp );
  glViewport( 0, 0, g_img_width, g_img_height );
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, g_img_width, 0, g_img_height );

  g_FBO->Bind();
  glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
  
  glBindTexture( GL_TEXTURE_RECTANGLE_ARB, g_MonoTex );
  one_pass( *g_hProgram, g_img_width );
  
  glBindTexture( GL_TEXTURE_RECTANGLE_ARB, g_ColorTex );
  //glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
  one_pass( *g_vProgram, g_img_height );
  
  glUseProgram( 0 );
  FramebufferObject::Disable();
  glViewport( vp[0], vp[1], vp[2], vp[3] );	
  glDrawBuffer( currentDrawBuf );
}

/* GLSL initialization*/
static void initShader( )
{
  ShaderObject hShader( GL_FRAGMENT_SHADER, "GaussianFilterH.glsl", true );
  ShaderObject vShader( GL_FRAGMENT_SHADER, "GaussianFilterV.glsl", true );
  ShaderObject sShader( GL_FRAGMENT_SHADER, "SelectChannel.glsl", true );
  
  g_hProgram = new ProgramGLSL();
  g_vProgram = new ProgramGLSL();
  g_sProgram = new ProgramGLSL();

  g_hProgram->AttachShaderObject( hShader );
  g_vProgram->AttachShaderObject( vShader );
  g_sProgram->AttachShaderObject( sShader );

  g_hProgram->LinkProgram();
  g_vProgram->LinkProgram();
  g_sProgram->LinkProgram();
}

static void draw()
{
  /* send elapsed time to shaders*/	
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  //glEnable( GL_TEXTURE_RECTANGLE_ARB );
  glBindTexture( GL_TEXTURE_RECTANGLE_ARB, g_ColorTex );

  GLint sTexParam = g_sProgram->GetUniformLocation( "Tex" );
  g_sProgram->UseProgram();
  glUniform1i( sTexParam, 0 );

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0, 0);                      glVertex2f(0, g_img_height );
    glTexCoord2f(g_img_width, 0);            glVertex2f( g_img_width, g_img_height);
    glTexCoord2f(g_img_width, g_img_height); glVertex2f( g_img_width, 0 );
    glTexCoord2f(0, g_img_height);           glVertex2f(0, 0 );
  }
  glEnd();
  //glDisable( GL_TEXTURE_RECTANGLE_ARB );

  glutSwapBuffers();
}

static void reshape(int width, int height)
{
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 27:
    case 'Q':
    case 'q':
      exit(EXIT_SUCCESS);
    default:
      break;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("please privide filename.\n");
    return 0;
  }
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
  glutInitWindowSize( 800, 600 );
  glutCreateWindow( "Simple GLSL example" );
  glutDisplayFunc( draw );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( keyboard);		

  clock_t t1 = clock();

  glewInit();	
  initialize(argv[1]);
  initShader( );	
  
  filter();

  clock_t t2 = clock();
  std::cout << ( t2 - t1 ) / 1000.0 << std::endl;

  glutMainLoop();
  return 0;
}

