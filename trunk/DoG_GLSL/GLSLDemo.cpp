// GLSLDemo.cpp : Defines the entry point for the console application.
//

/* GLSL demo program*/



#define GL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <ctime>

#include "framebufferObject.h"
#include <IL/il.h>
#include "glErrorUtil.h"
#include "GlobalUtil.h"
#include "ShaderObject.h"
#include "ProgramGLSL.h"
#include "GLTexImage.h"
#include "GLSLFilter.h"
#include "PyramidGLSL.h"

int g_img_width = 0;
int g_img_height = 0;
float g_sigma = 0.1f;

GLTexInput g_input;
PyramidGLSL* g_pyramid;
ProgramGLSL* sProgram;
int g_current = 0;
SiftParams g_sp;

static void ReshapeWindow( const int width, const int height ) {
  int v[4];
  glGetIntegerv( GL_VIEWPORT, v );
  if ( width == v[2] && height == v[ 3 ] )
    return;
  glutReshapeWindow( width, height );
}

/*standard OpenGL initialization*/
static bool initialize(const char *filename) {		
  if (!g_input.LoadImageFromFile(filename, false )) return false;
  sProgram = new ProgramGLSL("SelectChannel.glsl");	
  sProgram->AttachShaderObject( 
    ShaderObject( GL_FRAGMENT_SHADER, "SelectChannel.glsl", true ) );
  if (!sProgram->LinkProgram()) return false;
  
  g_img_width = g_input.GetImageWidth();
  g_img_height = g_input.GetDrawHeight();
  ReshapeWindow( g_img_width, g_img_height );	

  g_pyramid = new PyramidGLSL( &g_input, g_sp.GetSigmas(), g_sp.GetLevels() );
  return true;
}

static void draw()
{
  printf( "level = %d, sigma = %f\n", g_current, g_sp.GetSigmas()[ g_current ] );
  sProgram->UseProgram();
  g_pyramid->GetLevel( g_current )->DrawImage();
  glUseProgram( 0 );
  glutSwapBuffers();
  return;
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
  static float step = 0.5f;
  switch (key) {
    case 'N':
    case 'n':
      ++g_current;
      if ( g_current == g_sp.GetLevels() )
        --g_current;
      glutPostRedisplay();
      break;
    case 'P':
    case 'p':
      --g_current;
      if ( g_current < 0 )
        ++g_current;
      glutPostRedisplay();
      break;
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
    printf("please provide filename.\n");
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
  if (!initialize(argv[1])) {
    printf("initialize failed, press any key to exit!\n");
    getchar();
    exit(-1);
  }

  clock_t t2 = clock();
  std::cout << ( t2 - t1 ) / 1000.0 << std::endl;

  glutMainLoop();
  return 0;
}

