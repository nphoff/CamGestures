//Function will trace and display the contours in the picture.

#include <iostream>
#include <fstream>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include "colourslidebar.hpp"

#define CVX_RED		CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN	CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE	CV_RGB(0x00,0x00,0xff)

using namespace std;

int main(int argc, char** argv){
  IplImage* img = cvLoadImage( argv[1],CV_LOAD_IMAGE_GRAYSCALE );
  IplImage* grey = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
  IplImage* eroded = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
  IplImage* color = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
  CvMemStorage* storage = cvCreateMemStorage();
  CvMemStorage* cHStorage = cvCreateMemStorage();
  CvMemStorage* cHDStorage = cvCreateMemStorage();
  CvMemStorage* faceStorage = cvCreateMemStorage();
  CvSeq* contours = NULL;
  CvSeq* convexHull = NULL;
  CvSeq* convexHull2 = NULL;
  CvSeq* convexityDefects = NULL;
  CvSeq* faces;
  CvSize trainer_size = cvSize(20,20);
  CvHaarClassifierCascade* cascade = cvLoadHaarClassifierCascade("haarcascade_frontalface_alt2.xml",trainer_size);
  CvMoments moments;
  CvMoments* mooment; 

  cvNamedWindow("Raw Binary Image", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Eroded Image", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Contours", CV_WINDOW_AUTOSIZE);

  cvCopy(img,grey,NULL);
  cvErode(img, eroded,NULL,1);
  int Nc = cvFindContours(eroded, storage, &contours,
			  sizeof(CvContour),
			  CV_RETR_EXTERNAL
			  );
  cvCvtColor( grey, color, CV_GRAY2BGR);
  faces = cvHaarDetectObjects(grey,cascade,faceStorage,1.2,2,CV_HAAR_DO_CANNY_PRUNING);
  if(faces){
    cout << "Found a face." << endl;
    for(int i = 0; i< faces->total; i++){
      CvRect* face_rect = (CvRect*) cvGetSeqElem(faces,i);
      //add some output here.  THIS IS WHERE I WAS.
      CvPoint center_of_face = cvPoint((int)(face_rect->x + face_rect->width / 2),
				     (int)(face_rect->y + face_rect->height /2));
      cvRectangle(color,
		  cvPoint(face_rect->x, face_rect->y),
		  cvPoint(face_rect->x + face_rect->width, face_rect->y + face_rect->height),
		  CV_RGB(255,0,0),
		  3
		  );
      cvCircle(color, center_of_face, 20, CVX_GREEN, 2);

    }
  }

  int d = -1;
  int n = 0;
    
  cvShowImage("Raw Binary Image", grey);
  cvShowImage("Eroded Image", eroded);
  cvShowImage("Contours", color);
  cvWaitKey(0);
  for(CvSeq* c = contours; c!=NULL; c= c->h_next){
    if(c->total > 500){
      convexHull = cvConvexHull2(c, cHStorage, CV_CLOCKWISE, 2);
      convexHull2 = cvConvexHull2(c, cHDStorage, CV_CLOCKWISE, 0);
      convexityDefects = cvConvexityDefects(c, convexHull2);
      cvDrawContours(color,
		     convexHull,
		     CVX_GREEN,
		     CVX_BLUE,
		     0,
		     2,
		     8
		     );
      cvDrawContours(color,
		     c,
		     CVX_RED,
		     CVX_BLUE,
		     0,
		     2,
		     8
		     );
      
      cvMoments(convexHull,&moments);
      int k = 0;
      CvPoint center_of_mass = cvPoint((int)(moments.m10/moments.m00), (int)(moments.m01/moments.m00));
      cvCircle(color, center_of_mass, 20, CVX_GREEN, 3);
      for(int j = 0; j < convexityDefects -> total; ++j){
	CvConvexityDefect* cDef = (CvConvexityDefect*)cvGetSeqElem( convexityDefects,j);
	if(cDef->depth > 30){
	  k++;
	  cvCircle(color, *(cDef->depth_point), 5, CVX_BLUE, 2);
	}
      }
      
      
      cvShowImage("Contours", color);
      d = cvWaitKey(0);
    }
    if( d == 27){
      cout << "Escape key received, terminating." << endl;
      break;
    }
    n++;

  }
  return 0;
}
