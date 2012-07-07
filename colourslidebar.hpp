#ifndef __COLOURSLIDEBAR_HPP__
#define __COLOURSLIDEBAR_HPP__

#include "cv.h"
#include "highgui.h"

void colourslidebar(IplImage* masked,
		    int thresholds [],
		    const char* windowname,
		    IplImage* original
		    );



#endif
