#ifndef __LIB_DTMF_IN__
#define __LIB_DTMF_IN__

#define NUM_FREQS			8		// The number of dtmf frequencies (band pass filters)

struct FilterCoefficientsEntry
{
	double unityGainCorrection;
	double coeff1;
	double coeff2;
};

#endif /* end of include guard: __LIB_DTMF_IN__ */
