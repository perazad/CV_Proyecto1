#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

string ToUpperCase(char *word) {

	char *wordUC = new char[strlen(word)];
	

	for (int i = 0; i < strlen(word); i++)
	{
		
        wordUC[i] = (char)toupper(word[i]);

	}

	string rtnStr(wordUC);

	return rtnStr;

}


int main(int argc, char* argv[])
{

	bool ok;
	bool image = false;
	bool roi = true;

	bool video = false;
	bool camera = true;

	bool surf = true;
	bool sift = false;

	Mat frame;
	Mat img;

	printf("This program performs a feature point tracking using an image (-i or -I) or region of interest (-r or -R) - default as baseline.\n");
	printf("The program can run on a video (-v or -V) or using captures from webcam (-c or -C) - default.\n");
    printf("Press q in image window for exit.\n");

	//Parameters validation
	if(argc > 1) {

		if(ToUpperCase(argv[1]) == "-I") {
			image = true;
			roi = false;

			printf("if(ToUpperCase(argv[1]) == -I)\n");

			if(argc <= 2) {
				printf("Please specify image name with proper extension after parameter -i or -I\n");
				return -1;
			}
			else if(argc > 2) {

				if(ToUpperCase(argv[3]) == "-V") {
					video = true;
					camera = false;

					if(argc <= 4) {
						printf("Please specify video name with proper extension after parameter -v or -V\n");
						return -1;
					}
					else if(argc > 4) {
						
						if(ToUpperCase(argv[5]) == "-SU") {
							surf = true;
							sift = false;							
						}
						else if(ToUpperCase(argv[5]) == "-SI"){
							sift = true;
							surf = false;
						}
						else {
							printf("Please introduce a valid feature detector and descriptor to use: SURF (-su or SU), SIFT (-si or -SI)");
							return -1;
						}
					
					}
	
				}//if(ToUpperCase(argv[3]) == "-V")
				else if(ToUpperCase(argv[3]) == "-C") {
					camera = true;
					video = false;

					if(ToUpperCase(argv[4]) == "-SU") {
							surf = true;
							sift = false;							
					}
					else if(ToUpperCase(argv[4]) == "-SI"){
						sift = true;
						surf = false;
					}
					else {
						printf("Please introduce a valid feature detector and descriptor to use: SURF (-su or SU), SIFT (-si or -SI)");
						return -1;
					}

				}//else if(ToUpperCase(argv[3]) == "-C") 

			}//else if(argc > 2)

		}//if(ToUpperCase(argv[1]) == "-I")
		else if(ToUpperCase(argv[1]) == "-R") {
			roi = true;
			image = false;

			if(argc > 2) {

				if(ToUpperCase(argv[2]) == "-V") {
					video = true;
					camera = false;

					if(argc <= 3) {
						printf("Please specify video name with proper extension after parameter -v or -V");
						return -1;
					}
					else if(argc > 3) {
						
						if(ToUpperCase(argv[4]) == "-SU") {
							surf = true;
							sift = false;							
						}
						else if(ToUpperCase(argv[4]) == "-SI"){
							sift = true;
							surf = false;
						}
						else {
							printf("Please introduce a valid feature detector and descriptor to use: SURF (-su or SU), SIFT (-si or -SI)");
							return -1;
						}

					}
					
				}//if(ToUpperCase(argv[2]) == "-V")
				else if(ToUpperCase(argv[2]) == "-C") {
					camera = true;
					video = false;

					if(ToUpperCase(argv[3]) == "-SU") {
							surf = true;
							sift = false;							
					}
					else if(ToUpperCase(argv[3]) == "-SI"){
						sift = true;
						surf = false;
					}
					else {
						printf("Please introduce a valid feature detector and descriptor to use: SURF (-su or SU), SIFT (-si or -SI)");
						return -1;
					}
				}//else if(ToUpperCase(argv[2]) == "-C")
	
			}
		}

	}//if(argc > 1)

	VideoCapture cap;

	if(camera)
    	cap = VideoCapture(0); // open the default camera
	else
		cap = VideoCapture(argv[4]);
    
	if(!cap.isOpened())  // check if we succeeded
        return -1;

    //Mat edges;
	if(image)
		img = imread(argv[2], IMREAD_GRAYSCALE);

	if(roi) {
		// Read first frame     
    	ok = cap.read(frame); 

		// Define initial bounding box 
		Rect2d bbox = selectROI(frame, false); 

		//Crop image from frame
		img = frame(bbox);
	}

	if(!img.data) {
  		std::cout<< " --(!) Error reading image " << std::endl; 
		return -1;
	}

	//-- Step 1: Detect the keypoints using SURF Detector
  	int minHessian = 400;

  	Ptr<SURF> detectorSurf = SURF::create(minHessian);
	Ptr<SIFT> detectorSift = SIFT::create(minHessian);

  	vector<KeyPoint> keyPoints;
	Mat descriptor;

	if (surf)
  		detectorSurf->detectAndCompute(img, noArray(), keyPoints, descriptor);
	else if(sift)
		detectorSift->detectAndCompute(img, noArray(), keyPoints, descriptor);

  	//-- Draw keypoints
  	Mat imgKeyPoints;


	if (surf)
  		drawKeypoints(img, keyPoints, imgKeyPoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT); 
	else if(sift)
		drawKeypoints(img, keyPoints, imgKeyPoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT); 

  	//-- Show detected (drawn) keypoints
	if (surf)
  		imshow("Keypoints with SURF", imgKeyPoints);
	else if(sift)
		imshow("Keypoints with SIFT", imgKeyPoints);

	

    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
	
		Ptr<SURF> detectorSurf2 = SURF::create(minHessian);
		Ptr<SIFT> detectorSift2 = SIFT::create(minHessian);

		vector<KeyPoint> keyPoints2;
		Mat descriptor2;

		if(surf)
  			detectorSurf2->detectAndCompute(frame, noArray(), keyPoints2, descriptor2);
		else if(sift)
			detectorSift2->detectAndCompute(img, noArray(), keyPoints2, descriptor2);

  		//-- Draw keypoints
  		Mat imgKeyPoints2;

		if(surf)
  			drawKeypoints(frame, keyPoints2, imgKeyPoints2, Scalar::all(-1), DrawMatchesFlags::DEFAULT); 
		else if(sift)
			drawKeypoints(frame, keyPoints2, imgKeyPoints2, Scalar::all(-1), DrawMatchesFlags::DEFAULT); 

		//-- Step 2: Matching descriptor vectors with a FLANN based matcher
    	// Since SURF is a floating-point descriptor NORM_L2 is used
    	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    	vector< std::vector<DMatch> > knn_matches;
    	matcher->knnMatch(descriptor, descriptor2, knn_matches, 2 );

		//-- Filter matches using the Lowe's ratio test
    	const float ratio_thresh = 0.7f;
    	vector<DMatch> good_matches;
    	for (size_t i = 0; i < knn_matches.size(); i++)
    	{
    	    if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
    	    {
    	        good_matches.push_back(knn_matches[i][0]);
    	    }
    	}
    	//-- Draw matches
    	Mat img_matches;
    	drawMatches(img, keyPoints, frame, keyPoints2, good_matches, img_matches, Scalar::all(-1),
                 Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    	//-- Show detected matches
    	imshow("Proyecto01: Puntos de interes", img_matches );

        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);

        //imshow("Proyecto01: Puntos de interes", frame);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

