#ifndef __GAUSSIAN_H__
#define __GAUSSIAN_H__

#include <math.h>

template< typename T >
T* gaussian1D( const unsigned R, const T sigma ) {
	T* weights = new float[ R ];

	T sigma_2 = sigma * sigma;
	const T inv_two_sigma_squared = 0.5f / sigma_2;
	for ( unsigned weight = 0; weight < R; ++weight ) {
		const T x = T( weight * weight );
		weights[ weight ] = exp( - x * inv_two_sigma_squared );
	}

	//normalize
	T sum = 0;
	for ( unsigned int i = 1; i < R; ++i )
		sum +=  weights[ i ] + weights[ i ];
	sum += weights[0];
	for ( unsigned int i = 0; i < R; ++i )
		weights[ i ] /= sum;

	return weights;
}

#endif//__GAUSSIAN_H__
