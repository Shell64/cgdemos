#include <GL/glew.h>
#include <iostream>
#include <cmath>

#include "GLTexImage.h"
#include "ProgramGLSL.h"
#include "GLSLFilter.h"
#include "PyramidGLSL.h"

PyramidGLSL::PyramidGLSL( GLTexImage* input, double* sigmas, int levels )
: m_pyramid( NULL ), m_levels( levels )
{
	InitPyramid( input->GetImageWidth(), input->GetTexHeight() );
	BuildPyramid( input, sigmas );
}

PyramidGLSL::~PyramidGLSL(void)
{
	if ( NULL != m_pyramid )
	{
		delete[] m_pyramid;
		m_pyramid = NULL;
	}	
}

void PyramidGLSL::InitPyramid( int width, int height )
{
	m_pyramid = new GLTexImage[ m_levels ];
	if ( NULL == m_pyramid )
	{
		std::cout << "failed to allocate memory for pyramid.\n";
		return;
	}	

	for ( int i = 0; i < m_levels; ++i )
	{
		m_pyramid[ i ].InitTexture( width, height );
	}
}

void PyramidGLSL::BuildPyramid( GLTexImage* input, double* sigmas )
{
	if ( NULL == input || NULL == m_pyramid || NULL == sigmas )
		return;
	
	GLSLFilter filter;

	GLTexImage* src = input;
	for ( int i = 0; i < m_levels; ++i )
	{
		filter.CalculateGuassianKernel( sigmas[ i ] );
		GLTexImage* dst = &m_pyramid[ i ];
		filter.Run( src, dst );
		src = dst;
	}
}



SiftParams::SiftParams( int intvls /*= SIFT_INTVLS*/,
					   double sigma /*= SIFT_SIGMA*/, 
					   double contrThr /*= SIFT_CONTR_THR*/, 
					   int curvThr /*= SIFT_CURV_THR*/,
					   bool dblImg /*= SIFT_DBL_IMG*/, 
					   int descrWidth /*= SIFT_DESCR_WIDTH*/, 
					   int descrHinstBins /*= SIFT_DESCR_HIST_BINS */ )
					   : m_intvls( intvls ), m_sigmas( NULL ),
					   m_contrThr( contrThr ), m_cvrvThr( curvThr ),
					   m_dblImage( dblImg ), m_descrWidth( descrWidth ),
					   m_descrHinstBins( descrHinstBins )
{
	int levels = m_intvls + 2;
	m_sigmas = new double[ levels ];
	if ( NULL == m_sigmas )
	{
		std::cout << "failed to allocate memory for sigmas.\n";
	}
	
	double k = pow( 2., 1. / m_intvls );
	double sigma_prev, sigma_total;
	sigma_total = sigma;
	for ( int i = 0; i < levels; ++i )
	{
		sigma_prev = sigma_total;
		sigma_total = sigma_prev * k;
		m_sigmas[ i ] = sqrt( SQUARE( sigma_total ) - SQUARE( sigma_prev ) );
	}
}

SiftParams::~SiftParams()
{
	if ( NULL != m_sigmas )
	{
		delete m_sigmas;
		m_sigmas = NULL;
	}
}
