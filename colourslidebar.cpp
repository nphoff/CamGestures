//This function will add sliders to a masked image,
//Allowing the changing of thresholding to get a better mask.
//It will then destroy the sliders after a satisfactory mask has
//been created.

#include "colourslidebar.hpp"
#include "cv.h"
#include "highgui.h"

using namespace std;

void colourslidebar(IplImage* masked,
		   int thresholds [],
		   const char* windowname,
		   IplImage* original
		   ){

  int c;
  cvCreateTrackbar("Y Low", windowname, &thresholds[0],255);
  cvCreateTrackbar("Y High", windowname, &thresholds[3],255);
  cvCreateTrackbar("Cr Low", windowname, &thresholds[1],255);
  cvCreateTrackbar("Cr High", windowname, &thresholds[4],255);
  cvCreateTrackbar("Cb Low", windowname, &thresholds[2],255);
  cvCreateTrackbar("Cb High", windowname, &thresholds[5],255);
  cvShowImage(windowname, masked);
  
  
  while(1){
    cvInRangeS(original,
	      cvScalar(thresholds[0],
		       thresholds[1],
		       thresholds[2]
		       ),
	      cvScalar(thresholds[3],
		       thresholds[4],
		       thresholds[5]
		       ),
	      masked
	      );

    cvShowImage(windowname, masked);
    c = cvWaitKey(500);
    if(c==27){
      cout << "Esc key received, returning to normal." << endl;
      break;
    } else if(c == 115){
      cout << "Dumping current values to stdin: " <<endl;
      for(int i = 0; i< 6; i++){
	cout << "threshold " << i << " is: " << thresholds[i] << endl;
      }
    }
  }
}
  
