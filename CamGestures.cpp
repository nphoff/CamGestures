//Function to add slidebar functionality to webcam input to mask
//selective colours, and make certain things easier to pick out.
// Written by Nathan Hoffman -- July 3rd, 2012

#include <iostream>
#include <fstream>
#include <string>
#include "cv.h"
#include "highgui.h"
#include "colourslidebar.hpp"

#define CVX_RED		CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN	CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE	CV_RGB(0x00,0x00,0xff)

using namespace std;

void DisplayMenu(void);
bool IsWithinRect(CvPoint a_point, CvRect bounding_rect);

int main(int argc, char** argv){
  const char* windowname = "Mask";
  cvNamedWindow("Raw Webcam Output", CV_WINDOW_AUTOSIZE);
  cvNamedWindow(windowname, CV_WINDOW_AUTOSIZE);
  //Thresholds is an array of YCrCb low then high thresholds for mask
  int thresholds [6] = {27,85,115,75,135,154};
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
  CvMemStorage* storage = cvCreateMemStorage();
  CvMemStorage* cHStorage = cvCreateMemStorage();
  CvMemStorage* cHDStorage = cvCreateMemStorage();
  CvMemStorage* face_storage = cvCreateMemStorage();
  //CvMemStorage* smooth_storage = cvCreateMemStorage();
  CvSeq* contours;
  //CvSeq* smoothed_contours;
  CvSeq* convexHull = NULL;
  CvSeq* convexHull2 = NULL;
  CvSeq* convexityDefects = NULL;
  CvConvexityDefect* cDef = NULL;
  CvSeq* faces = NULL;
  CvRect* face_rect;
  CvPoint* face_center;
  CvPoint center_of_face;
  CvPoint center_of_mass;
  bool face_defined = false;
  CvMoments moments;
  int nc, n, d;
  int frame_counter = 0;
  CvSize trainer_size = cvSize(20,20);
  CvHaarClassifierCascade* cascade = cvLoadHaarClassifierCascade("haarcascade_frontalface_alt2.xml",trainer_size);

  DisplayMenu();
  while(1){
    frame_counter++;
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
    contours = NULL;
    if((frame_counter) % 600 == 0|| frame_counter == 15){
      faces = cvHaarDetectObjects(frame,cascade,face_storage,1.2,CV_HAAR_DO_CANNY_PRUNING);
      if(faces){
	face_defined = true;
	for(int i = 0; i< faces->total; i++){
	  face_rect = (CvRect*) cvGetSeqElem(faces,i);
	  center_of_face = cvPoint((int)(face_rect->x + face_rect->width / 2),
					   (int)(face_rect->y + face_rect->height /2));
	}
      } else {
	face_defined = false;
      }
    }
    nc = cvFindContours(temp, storage, &contours,
			    sizeof(CvContour),
			    CV_RETR_EXTERNAL
			    );
    if(face_defined == true){
      cvCircle(frame, center_of_face, 20, CV_RGB(0xff,0xff,0xff), 2);
    }
    for(CvSeq* oc = contours; oc!=NULL; oc= oc->h_next){
      if(oc->total > 450){
	cvMoments(oc,&moments);
	center_of_mass = cvPoint((int)(moments.m10/moments.m00), 
				 (int)(moments.m01/moments.m00));
	if(!(face_defined == true && IsWithinRect(center_of_mass, *(face_rect)))){
		convexHull = cvConvexHull2(oc, cHStorage, CV_CLOCKWISE, 2);
		convexHull2 = cvConvexHull2(oc, cHDStorage, CV_CLOCKWISE, 0);
		convexityDefects = cvConvexityDefects(oc, convexHull2);
		
		cvDrawContours(frame,
			       convexHull,
			       CVX_GREEN,
			       CVX_BLUE,
			       0,
			       2,
			       8
			       );
		cvDrawContours(frame,
			       oc,
			       CVX_RED,
			       CVX_BLUE,
			       0,
			       2,
			       8
			       );
		cvCircle(frame, center_of_mass, 20, CVX_GREEN, 3);
		for(int j = 0; j < convexityDefects -> total; ++j){
		  cDef = CV_GET_SEQ_ELEM( CvConvexityDefect, convexityDefects,j);
		  if(cDef->depth > 30){
		    cvCircle(frame, *(cDef->depth_point), 5, CVX_BLUE, 2);
		  }
		}
		
	}
	//smoothed_contours = cvApproxPoly(oc,sizeof(CvContour),smooth_storage,CV_POLY_APPROX_DP,5);
      }
    }
    
    contours = NULL;
    
    cvShowImage("Raw Webcam Output", frame);
    cvShowImage(windowname, masked);
    c = cvWaitKey(33);
    if(c == 27) break;
    if(c == 115){
      // 's' -- open the colour slidebar
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


void DisplayMenu(void){
  cout << endl << string(76,'*') << endl
       << "Welcome to the Cam Gestures Program!" << endl
       << string(76,'*') << endl
       << "Current supported commands are: " << endl
       << "d -- > Save images" << endl
       << "s -- > Modify the mask with the colour slidebar" << endl
       << "Esc  > Terminate the program" << endl;
}

bool IsWithinRect(CvPoint a_point, CvRect bounding_rect){
  if(bounding_rect.x < a_point.x &&
     bounding_rect.y < a_point.y &&
     bounding_rect.x + bounding_rect.width > a_point.x &&
     bounding_rect.y + bounding_rect.height > a_point.y){
    return true;
  } return false;
}
