
#include "average_float.h"

// Constructor
AverageFloat::AverageFloat(uint16_t sampleSize) {
	this->anz		= sampleSize;
	this->avgsum	= 0;
	this->n			= 0;
}


void AverageFloat::append(double newval) {
  if (this->n < this->anz)
  {
    this->n++;
    this->avgsum += newval;
  }
  else
  {
    this->avgsum -= this->avgsum/this->anz;
    this->avgsum += newval;
  }
}

double AverageFloat::avg(void) {
	if (this->n < this->anz) 
		return this->avgsum/this->n;
	else         
		return this->avgsum/this->anz;
}
