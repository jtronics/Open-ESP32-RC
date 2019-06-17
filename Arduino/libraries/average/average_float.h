#ifndef _AVERAGEFLOAT_H
#define _AVERAGEFLOAT_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


class AverageFloat {
	
private:
	uint16_t	anz=0;
	uint16_t	n=0;
	double		avgsum=0;

public: 
	// create a Filter that contains up to specified number of values
	AverageFloat(uint16_t sampleSize =8);

	// put a new value into the Filter object, discarding oldest value if necessary
	void append(double value);

	// return the mean, average of values currently in Filter object
	double avg(void);

};

#endif	// _AVERAGEFLOAT_H
