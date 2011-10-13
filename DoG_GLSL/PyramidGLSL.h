#ifndef __PYRAMID_H__
#define __PYRAMID_H__

/** default number of sampled intervals per octave */
#define SIFT_INTVLS 3

/** default sigma for initial gaussian smoothing */
#define SIFT_SIGMA 1.6

/** default threshold on keypoint contrast |D(x)| */
#define SIFT_CONTR_THR 0.04

/** default threshold on keypoint ratio of principle curvatures */
#define SIFT_CURV_THR 10

/** double image size before pyramid construction? */
#define SIFT_DBL_IMG true

/** default width of descriptor histogram array */
#define SIFT_DESCR_WIDTH 4

/** default number of bins per histogram in descriptor array */
#define SIFT_DESCR_HIST_BINS 8

/************************************************************************
 * utilities
 ************************************************************************/
#define SQUARE( x ) (x) * (x)

#define DBL( x ) (x) * 2

#define DBL_I( x ) (x) << 1

#define HALF( x ) (x) / 2;

#define HALF_I( x ) (x) >> 1

#define ABS( x ) ( ( x ) < 0 ? -x : x  )

class GLTexImage;

class SiftParams
{
public:	
	SiftParams( int intvls = SIFT_INTVLS,
		double sigma = SIFT_SIGMA, double contrThr = SIFT_CONTR_THR,
		int curvThr = SIFT_CURV_THR, bool dbImg = SIFT_DBL_IMG, 
		int descrWidth = SIFT_DESCR_WIDTH, int descrHinstBins = SIFT_DESCR_HIST_BINS );

	~SiftParams();

public:
	inline int GetIntervals() { return m_intvls; }

	inline double* GetSigmas() { return m_sigmas; }

	inline int GetLevels() { return m_intvls + 2; }

protected:
	int m_intvls;	
	double m_contrThr; //contrast threshold
	int m_cvrvThr;
	bool m_dblImage;
	int m_descrWidth;
	int m_descrHinstBins;
	double* m_sigmas;
};

class PyramidGLSL
{
public:
	PyramidGLSL( GLTexImage* input, double* sigmas, int levels );

	~PyramidGLSL(void);	

public:

	inline GLTexImage* GetLevel( int level )
	{
		if ( level < 0 || level >= m_levels ) return NULL;
		return &m_pyramid[ level ];
	}

protected:
	
	void BuildPyramid( GLTexImage* input, double* sigmas );

	void InitPyramid( int width, int height );

protected:
	GLTexImage* m_pyramid;
	int m_octvs;
	int m_levels;
};

#endif//__PYRAMID_H__
