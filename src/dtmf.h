//
//  DTMF utils
//  libdtmf
//
//  Created by jens on 12.04.13.
//  Copyleft (ɔ) 2013 jens alexander ewald. GPL applies.
//

#ifndef __LIB_DTMF__
#define __LIB_DTMF__

#include <stddef.h>

#ifndef SAMPLING_RATE
  #define SAMPLING_RATE		8000
#endif


#define MIN_TONE_LENGTH		0.010	// 45ms
#define FRAMES_PER_TONE		1
#define BYTES_PER_CHANNEL	2
#define BUFFER_SIZE			((int)(MIN_TONE_LENGTH * SAMPLING_RATE * BYTES_PER_CHANNEL) / FRAMES_PER_TONE )

// #define NO_CODE 255
const char NO_CODE = '\0';

#define kDefaultNoiseToleranceFactor 2.5

// 0 = Peak Value -> RMS, 1 = Sqrt of Sum of Squares, 2 = Sum of Abs Values
enum PowerMeasurementMethod {
  RMS,
  SQRT,
  ABS
};

void DTMFDecode(const void* buffer, size_t buffer_size,char *code);

void DTMFSetup(int sampling_rate, int frame_size);

void* buffer_for(char c);

#endif /* defined(__LIB_DTMF__) */