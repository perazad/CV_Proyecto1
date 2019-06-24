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


int main(int argc, char* argv[])
{

	bool ok;
	bool image = false;
	bool roi = true;

	bool video = false;
	bool camera = true;

	bool surf = true;
	bool sift = false;
	bool akaze = false;
	bool kaze = false;	

	Mat frame;
	Mat img;

	Ptr<DescriptorMatcher> matcherFLANN;
	BFMatcher matcherBF(NORM_HAMMING);

	printf("This program performs a feature point tracking using an image (-i or -I) or region of interest (-r or -R) - default as baseline.\n");
	printf("The program can run on a video (-v or -V) or using captures from webcam (-c or -C) - default.\n");
	printf("The program use sevaral feature descriptors like: surf (-su or -SU) - default, sift (-si or -SI), akaze (-ak or -AK) and kaze (-kz or -KZ).\n");
	printf("Parameters must be send in order when program is executed.\n"); 
    printf("Press q in image window for exit.\n");

	//Parameters validation
	if(argc > 1) {

		string inic(argv[1]);

		if(inic == "-I" || inic == "-i" ) {
			image = true;
			roi = false;

			if(argc <= 2) {
				printf("Please specify image name with proper extension after parameter -i or -I\n");
				return -1;
			}
			else if(argc > 2) {

				string source(argv[3]);

				if(source == "-V" || source == "-v") {
					video = true;
					camera = false;

					if(argc <= 4) {
						printf("Please specify video name with proper extension after parameter -v or -V\n");
						return -1;
					}
					else if(argc > 4) {

						string method(argv[5]);
						
						if(method == "-SU" || method == "-su") {
							surf = true;
							sift = false;	
							akaze = false;
							kaze = false;						
						}
						else if(method == "-SI" || method == "-si"){
							sift = true;
							surf = false;
							akaze = false;
							kaze = false;	
						}
						else if(method == "-AK" || method == "-ak"){
							akaze = true;
							surf = false;
							sift = false;
							kaze = false;	
						}
						else {
							printf("Please introduce a valid feature detector and descriptor to use: SURF (-su or SU), SIFT (-si or -SI), AKANE (-ak or -AK)\n");
							return -1;
						}
					
					}
	
				}//if(ToUpperCase(argv[3]) == "-V")
				else if(source  == "-C" || source  == "-c") {
					camera = true;
					video = false;

					string method(argv[4]);

					if(method == "-SU" || method == "-su") {
							surf = true;
							sift = false;
							akaze = false;
							kaze = false;								
					}
					else if(method == "-SI" || method == "-si"){
						sift = true;
						surf = false;
						akaze = false;
						kaze = false;	
					}
					else if(method == "-AK" || method == "-ak"){
							akaze = true;
							surf = false;
							sift = false;
							kaze = false;	
					}
					else {
						printf("Please introduce a valid feature detector and descriptor to use: SURF (-su or SU), SIFT (-si or -SI), AKAZE (-ak or -AK)\n");
						return -1;
					}

				}//else if(ToUpperCase(argv[3]) == "-C") 

			}//else if(argc > 2)

		}//if(ToUpperCase(argv[1]) == "-I")
		else if(inic == "-R" || inic == "-r") {
			roi = true;
			image = false;

			if(argc > 2) {

				string source(argv[2]);

				if(source == "-V" || source == "-v") {
					video = true;
					camera = false;

					if(argc <= 3) {
						printf("Please specify video name with proper extension after parameter -v or -V");
						return -1;
					}
					else if(argc > 3) {

						string method(argv[4]);
						
						if(method == "-SU" || method == "-su") {
							surf = true;
							sift = false;	
							akaze = false;
							kaze = false;						
						}
						else if(method == "-SI" || method == "-si"){
							sift = true;
							surf = false;
							akaze = false;
							kaze = false;
						}
						else if(method == "-AK" || method == "-ak"){
							akaze = true;
							surf = false;
							sift = false;
							kaze = false;	
						}
						else {
							printf("Please introduce a valid feature detector and descriptor to use: SURF (-su or SU), SIFT (-si or -SI), AKAZE (-ak or -AK)\n");
							return -1;
						}

					}
					
				}//if(ToUpperCase(argv[2]) == "-V")
				else if(source == "-C" || source == "-c") {
					camera = true;
					video = false;

					string method(argv[3]);

					if(method == "-SU" || method == "-su") {
							surf = true;
							sift = false;
							akaze = false;
							kaze = false;							
					}
					else if(method == "-SI" || method == "-si"){
						sift = true;
						surf = false;
						akaze = false;
						kaze = false;
					}
					else if(method == "-AK" || method == "-ak"){
							akaze = true;
							surf = false;
							sift = false;
							kaze = false;	
					}
					else {
						printf("Please introduce a valid feature detector and descriptor to use: SURF (-su or SU), SIFT (-si or -SI), AKAZE (-AK or -ak");
						return -1;
					}
				}//else if(ToUpperCase(argv[2]) == "-C")
	
			}
		}

	}//if(argc > 1)

	VideoCapture cap;

	if(camera)
    	cap = VideoCapture(0); // open the default camera
	else if(image)
		cap = VideoCapture(argv[4]);
	else
		cap = VideoCapture(argv[3]);
    
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
	Ptr<AKAZE> detectorAkaze = AKAZE::create();

  	vector<KeyPoint> keyPoints;
	Mat descriptor;

	if (surf)
  		detectorSurf->detectAndCompute(img, noArray(), keyPoints, descriptor);
	else if(sift)
		detectorSift->detectAndCompute(img, noArray(), keyPoints, descriptor);
	else if(akaze)
		detectorAkaze->detectAndCompute(img, noArray(), keyPoints, descriptor);


  	//-- Draw keypoints
  	Mat imgKeyPoints;

	drawKeypoints(img, keyPoints, imgKeyPoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT); 

  	//-- Show detected (drawn) keypoints
  	imshow("Keypoints used", imgKeyPoints);	

    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
	
		Ptr<SURF> detectorSurf2 = SURF::create(minHessian);
		Ptr<SIFT> detectorSift2 = SIFT::create(minHessian);
		Ptr<AKAZE> detectorAkaze2 = AKAZE::create();

		vector<KeyPoint> keyPoints2;
		Mat descriptor2;

		if(surf)
  			detectorSurf2->detectAndCompute(frame, noArray(), keyPoints2, descriptor2);
		else if(sift)
			detectorSift2->detectAndCompute(frame, noArray(), keyPoints2, descriptor2);
		else if(akaze)
			detectorAkaze2->detectAndCompute(frame, noArray(), keyPoints2, descriptor2);

  		//-- Draw keypoints
  		Mat imgKeyPoints2;

  		drawKeypoints(frame, keyPoints2, imgKeyPoints2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

		if(surf || sift) {
			//-- Step 2: Matching descriptor vectors with a FLANN based matcher
    		// Since SURF and SIFT is a floating-point descriptor NORM_L2 is used
    		matcherFLANN = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
		}
	
    	vector< std::vector<DMatch> > knn_matches;

		if(surf || sift)
    		matcherFLANN->knnMatch(descriptor, descriptor2, knn_matches, 2);
		else
			matcherBF.knnMatch(descriptor, descriptor2, knn_matches, 2);

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


