//
//  libdtmf – dtmfin
//
//  Created by jens on 12.04.13.
//  Copyleft (ɔ) 2013 jens alexander ewald. GPL applies.
//

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "dtmf.h"
#include "dtmfin.h"


typedef int bool;
#define false 0x00
#define true  0x01

#define kMinNoiseTolerenceFactor	1.5
#define kMaxNoiseTolerenceFactor	6.5
#define kDefaultPowerMeasurementMethod 0x00


static double	powers[NUM_FREQS];		// Location to store the powers for all the frequencies
static double	filterBuf0[NUM_FREQS];	// Buffer for the IIR filter slot 0
static double	filterBuf1[NUM_FREQS];	// Buffer for the IIR filter slot 1

static int SamplingRate = SAMPLING_RATE;
static int BufferSize = 360;

static double	noiseTolerenceFactor   = kDefaultNoiseToleranceFactor;
static enum PowerMeasurementMethod pmm = kDefaultPowerMeasurementMethod;

// Filter coefficients

	//697 Hz
	//770 Hz
	//852 Hz
	//941 Hz
	//1209 Hz
	//1336 Hz
	//1477 Hz
  //1633 Hz

static struct FilterCoefficientsEntry COEF_8000[NUM_FREQS] = {
	{0.002729634465943104,	1.703076309365611,	0.994540731068114	  },
	{0.003014658069540622,	1.640321076289727,	0.9939706838609188	},
	{0.003334626751652912,	1.563455998285116,	0.9933307464966943	},
	{0.003681676706860666,	1.472762296913335,	0.9926366465862788	},
	{0.00472526211613835,	  1.158603326387692,  0.9905494757677233	},
	{0.005219030413485968,	0.991170124246961,	0.9895619391730281	},
	{0.005766653227008568,	0.7940130339147109,	0.9884666935459827	},
	{0.006371827557152033,	0.5649101144069607,	0.9872563448856961	}	
};

static struct FilterCoefficientsEntry COEF_11000[NUM_FREQS] = {
	{0.002729634465943104,	1.703076309365611,	0.994540731068114	  },
	{0.003014658069540622,	1.640321076289727,	0.9939706838609188	},
	{0.003334626751652912,	1.563455998285116,	0.9933307464966943	},
	{0.003681676706860666,	1.472762296913335,	0.9926366465862788	},
	{0.00472526211613835,	  1.158603326387692,  0.9905494757677233	},
	{0.005219030413485968,	0.991170124246961,	0.9895619391730281	},
	{0.005766653227008568,	0.7940130339147109,	0.9884666935459827	},
	{0.006371827557152033,	0.5649101144069607,	0.9872563448856961	}	
};

static struct FilterCoefficientsEntry COEF_22000[NUM_FREQS] = {
	{0.002729634465943104,	1.703076309365611,	0.994540731068114	  },
	{0.003014658069540622,	1.640321076289727,	0.9939706838609188	},
	{0.003334626751652912,	1.563455998285116,	0.9933307464966943	},
	{0.003681676706860666,	1.472762296913335,	0.9926366465862788	},
	{0.00472526211613835,	  1.158603326387692,  0.9905494757677233	},
	{0.005219030413485968,	0.991170124246961,	0.9895619391730281	},
	{0.005766653227008568,	0.7940130339147109,	0.9884666935459827	},
	{0.006371827557152033,	0.5649101144069607,	0.9872563448856961	}	
};

static struct FilterCoefficientsEntry COEF_44100[NUM_FREQS] = {
	{0.0004962819089923797,	1.98915881134097, 0.999007436192231	},
	{0.000548232471609901 ,	1.98688669890757, 0.998903537453566	},
	{0.0006065806043009561,	1.98407846099164, 0.998786842167417	},
	{0.000669900250714023 ,	1.98072442541614, 0.998660200516563	},
	
	{0.0008605252295523116,	1.96870650852839, 0.99827894959388  },
	{0.000950834456188125 ,	1.96200968184477, 0.998098333010697	},
	{0.0010510779663141   ,	1.95382373333843, 0.997897844218467	},
	{0.001161963129039151 ,	1.94385033380112, 0.997676073757652 }	
};

static struct FilterCoefficientsEntry COEF_48000[NUM_FREQS] = {
	{0.002729634465943104,	1.703076309365611,	0.994540731068114	  },
	{0.003014658069540622,	1.640321076289727,	0.9939706838609188	},
	{0.003334626751652912,	1.563455998285116,	0.9933307464966943	},
	{0.003681676706860666,	1.472762296913335,	0.9926366465862788	},
	{0.00472526211613835,	  1.158603326387692,  0.9905494757677233	},
	{0.005219030413485968,	0.991170124246961,	0.9895619391730281	},
	{0.005766653227008568,	0.7940130339147109,	0.9884666935459827	},
	{0.006371827557152033,	0.5649101144069607,	0.9872563448856961	}	
};

static struct FilterCoefficientsEntry COEF_96000[NUM_FREQS] = {
	{0.002729634465943104,	1.703076309365611,	0.994540731068114	  },
	{0.003014658069540622,	1.640321076289727,	0.9939706838609188	},
	{0.003334626751652912,	1.563455998285116,	0.9933307464966943	},
	{0.003681676706860666,	1.472762296913335,	0.9926366465862788	},
	{0.00472526211613835,	  1.158603326387692,  0.9905494757677233	},
	{0.005219030413485968,	0.991170124246961,	0.9895619391730281	},
	{0.005766653227008568,	0.7940130339147109,	0.9884666935459827	},
	{0.006371827557152033,	0.5649101144069607,	0.9872563448856961	}	
};

static struct FilterCoefficientsEntry* filterCoefficients = COEF_8000;


const char dtmfCodes[4][4] =
{
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'},
};

void DTMFSetup(int sampling_rate, int buffer_size) {
  switch(sampling_rate) {
    case 96000:
      filterCoefficients = COEF_96000;
      break;
    case 48000:
      filterCoefficients = COEF_48000;
      break;
    case 44100:
      filterCoefficients = COEF_44100;
      break;
    case 22000:
      filterCoefficients = COEF_22000;
      break;
    case 11000:
      filterCoefficients = COEF_11000;
      break;
    default:
    case 8000:
      filterCoefficients = COEF_8000;
      break;
  }
  SamplingRate = sampling_rate;
  BufferSize = buffer_size;
}

void setPowerMeasurementMethod(enum PowerMeasurementMethod m) {
  pmm = m;
}

void setDefaultPowerMeasurementMethod() {
  pmm = kDefaultPowerMeasurementMethod;
}

void setNoiseTolerance(float noiseLevel) {
	if (noiseLevel <= 0 || noiseLevel >1) noiseLevel = 0.5;
	noiseTolerenceFactor	= (double)(((1.0 - noiseLevel) * (kMaxNoiseTolerenceFactor - kMinNoiseTolerenceFactor)) + kMinNoiseTolerenceFactor);
}

void setDefaultNoiseTolerance() {
  noiseTolerenceFactor = kDefaultNoiseToleranceFactor;
}

// BpRe/100/frequency == Bandpass resonator, Q=100 (0=>Inf), frequency
// e.g. ./fiview 8000 -i BpRe/100/1336
// Generated using  http://uazu.net/fiview/
double bandPassFilter(register double val, int filterIndex)
{
	register double tmp, fir, iir;
	tmp= filterBuf0[filterIndex];
	filterBuf0[filterIndex] = filterBuf1[filterIndex];
	val *= filterCoefficients[filterIndex].unityGainCorrection;
	iir = val+filterCoefficients[filterIndex].coeff1 * filterBuf0[filterIndex] - filterCoefficients[filterIndex].coeff2 * tmp;
	fir = iir-tmp;
	filterBuf1[filterIndex] = iir;
	val = fir;
	return val;
}


char validate_code()
{
	// Find the highest powered frequency index
	int max1Index = 0;
	for (int i=0; i<NUM_FREQS; i++) {
		if ( powers[i] >= powers[max1Index] ) max1Index = i;
	}
	
	// Find the 2nd highest powered frequency index
	int max2Index;
	
	if ( max1Index == 0 ) {
		max2Index = 1;
	} else {
		max2Index = 0;
	}
	
	for (int i=0; i<NUM_FREQS; i++) {
		if (( powers[i] >= powers[max2Index] ) && ( i != max1Index )) max2Index = i;
	}
	
	// Check that fequency 1 and 2 are substantially bigger than any other frequencies
	bool valid = true;
	for (int i=0; i<NUM_FREQS; i++) {
		if (( i == max1Index ) || ( i == max2Index ))	continue;
		
		if (powers[i] > ( powers[max2Index] / noiseTolerenceFactor )) {valid = false;break;}
	}
	
	if ( valid ) {
		//printf("Highest Frequencies found: %d %d\n", max1Index, max2Index);
		
		// Figure out which one is a row and which one is a column
		int row = -1;
		int col = -1;
		if (( max1Index >= 0 ) && ( max1Index <=3 ))	{
			row = max1Index;
		} else	{
			col = max1Index;
		}
		
		if (( max2Index >= 4 ) && ( max2Index <=7 ))	{
			col = max2Index;
		} else {
			row = max2Index;
		}
		
		// Check we have both the row and column and fail if we have 2 rows or 2 columns
		if (( row == -1 ) || ( col == -1 )) {
			// We have to rows or 2 cols, fail
			return NO_CODE;
		} else {
			//printf("DTMFcodey %c\n",dtmfCodes[row][col-4]);
			return dtmfCodes[row][col-4];		// We got it
		}
	}
	return NO_CODE;
}

void normalize_short(short* buffer, size_t size) {
	short min,max;
  size_t i;

	// Normalize - AKA Automatic Gain
	min=buffer[0]; max=buffer[0];
	for (i=0L; i<size; i++) {
		if ( buffer[i] < min )	min = buffer[i];
		if ( buffer[i] > max )	max = buffer[i];
	}
  
	min = abs(min);
	max = abs(max);
	
	if ( max < min )	max = min;			// Pick bigger of max and min
	
	for (i=0L; i<size; i++)	{
		buffer[i] = (short)(((double)buffer[i] / (double)max) * (double)32767);
	}
}

int filter(const void* buffer, size_t size) {
	int t; double val;
  short* p = (short*)buffer;
  
  normalize_short(p,size);
  
  if(p==NULL) {
    return -1;
  }
	
	// Reset all previous power calculations
  for (t=0; t< NUM_FREQS; t++) {
   powers[t] = (double)0.0;
  }
	
	// Run the bandpass filter and calculate the power
	for (size_t i=0L; i<size; i++)	 {
		for (t=0; t< NUM_FREQS; t++) {
			
			// Find the highest value
			switch(pmm) {
				case RMS:
					val = fabs(bandPassFilter((double)p[i], t));
					if ( val > powers[t] )	powers[t] = val;
					break;
				case SQRT:
					val = bandPassFilter((double)p[i], t);
					powers[t] += val * val;
					break;
				case ABS:
				default:
					powers[t] += fabs(bandPassFilter((double)p[i], t));
					break;
			}
		}
	}
	
	// Scale 0 - 1, then convert into an power value
	
	for (t=0; t<NUM_FREQS; t++) {
		switch ( pmm ) {
			case RMS:
				powers[t] = (powers[t] / (double)32768.0) * ((double)1.0 / sqrt((double)2.0));
				break;
			case SQRT:
				powers[t] = sqrt(powers[t] / (double)size) / (double)32768.0;
				break;
			case ABS:
			default:
				powers[t] = (powers[t] / (double)size) / (double)32768.0;
				break;
		}
	}
	
	/*
	printf("RMS Powers: %0.3lf\t%0.3lf\t%0.3lf\t%0.3lf\t%0.3lf\t%0.3lf\t%0.3lf\t%0.3lf\n",
	      powers[0], powers[1], powers[2], powers[3],
	      powers[4], powers[5], powers[6], powers[7]);
	*/
  return 0;
}

void DTMFDecode(const void* buffer, const size_t size, char *code) {
  *code = filter(buffer,size) < 0 ? NO_CODE : validate_code();
}

