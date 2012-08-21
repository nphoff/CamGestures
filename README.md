Gesture interpretation program built in C++ using openCV

The evneutual goal of the project is to read in cued speech and output
the phonemes associated with gesture and mouth shape combinations.

The openCV library must be installed on your machine for this to work
Additionally, the V4L2 library (video for linux 2) must be installed to
read in the webcam data stream.

The current method is as follows:

* Haar Classifier Image detection (facial recognition)
* YCbCr Colourspace filtering (for skin tones)
* Center of mass estimates for masks (differentiate hand -- green circle, and face -- white circle)
* Convex Hull modelling (hand -- green contour)
* Convex Hull defecit (hand -- blue circles)

![Cam Gestures Webcam Stream Snapshot](http://i.imgur.com/vRc2Q.jpg "Showing different features currently implemented")


