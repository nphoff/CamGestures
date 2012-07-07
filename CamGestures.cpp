//Function to add slidebar functionality to webcam input to mask
//selective colours, and make certain things easier to pick out.
// Written by Nathan Hoffman -- July 3rd, 2012

#include <iostream>
#include <fstream>
#include "cv.h"
#include "highgui.h"
#include "colourslidebar.hpp"

using namespace std;



int main(int argc, char** argv){
  const char* windowname = "Mask";
  cvNamedWindow("Raw Webcam Output", CV_WINDOW_AUTOSIZE);
  cvNamedWindow(windowname, CV_WINDOW_AUTOSIZE);
  //Thresholds is an array of YCrCb low then high thresholds for mask
  int thresholds [6] = {80,85,135,255,135,180};
  IplImage* frame;
  int c;
  CvCapture* capture = cvCaptureFromCAM(-1);
  if(!capture){
    cout << "Error communicating with webcam." << endl
	 << "Terminating program." << endl;
    return -1;
  }
  frame = cvQueryFrame(capture);
  IplImage* masked = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U,1);
  cvSetIdentity(masked);
  IplImage* converted = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
  IplImage* temp = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
  CvMemStorage* storage;
  CvSeq* contours;
  int Nc, n, d;

  while(1){
    frame = cvQueryFrame(capture);
    if(!frame) break;
    cvCvtColor(frame,converted,CV_RGB2YCrCb);
    cvInRangeS(converted,
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
    cvErode(converted, converted);
    cvErode(converted, converted);
    cvSmooth(converted,converted);
    cvCopy(masked,temp,NULL);
    storage = cvCreateMemStorage();
    contours = NULL;
    Nc = cvFindContours(temp, storage, &contours,
			    sizeof(CvContour),
			    CV_RETR_EXTERNAL
			    );
    
    cvReleaseMemStorage(&storage);
    contours = NULL;
    
    cvShowImage("Raw Webcam Output", frame);
    cvShowImage(windowname, masked);
    c = cvWaitKey(33);
    if(c == 27) break;
    if(c == 115){
      colourslidebar(masked, thresholds, windowname, converted);
      cvDestroyWindow(windowname);
    } else if (c == 100){
      //unicode for 'd' -- dump images to files.
      cout << "dumping images" << endl;
      cvSaveImage("rawimage.jpg", frame);
      cvSaveImage("maskedimage.jpg", masked);

    }else if(c != -1){
      cout << "Received Key: " << c << endl;

    }
  }
  cvReleaseCapture(&capture);
  cvDestroyWindow("Raw Webcam Output");
  return 0;

}
