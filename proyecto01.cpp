#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
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
	Mat imgObj;

	Ptr<DescriptorMatcher> matcherFLANN;
	BFMatcher matcherBF(NORM_HAMMING);

	printf("This program performs a feature point tracking using an image (-i or -I) or region of interest (-r or -R) - default as baseline.\n");
	printf("The program can run on a video (-v or -V) or using captures from webcam (-c or -C) - default.\n");
	printf("The program use sevaral feature descriptors like: surf (-su or -SU) - default, sift (-si or -SI), akaze (-ak or -AK).\n");
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
						/*else if(method == "-KZ" || method == "-kz"){
							kaze = true;
							surf = false;
							sift = false;
							akaze = false;	
						}*/
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
					/*else if(method == "-KZ" || method == "-kz"){
							kaze = true;
							surf = false;
							sift = false;
							akaze = false;	
					}*/
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
						/*else if(method == "-KZ" || method == "-kz"){
							kaze = true;
							surf = false;
							sift = false;
							akaze = false;	
						}*/
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
					/*else if(method == "-KZ" || method == "-kz"){
							kaze = true;
							surf = false;
							sift = false;
							akaze = false;	
					}*/
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
		imgObj = imread(argv[2], IMREAD_GRAYSCALE);

	if(roi) {
		// Read first frame     
    	ok = cap.read(frame); 

		// Define initial bounding box 
		Rect2d bbox = selectROI(frame, false); 

		//Crop image from frame
		imgObj = frame(bbox);
	}

	if(!imgObj.data) {
  		std::cout<< " --(!) Error reading image " << std::endl; 
		return -1;
	}

	//-- Step 1: Detect the keypoints using SURF Detector
  	int minHessian = 400;

  	Ptr<SURF> detectorSurfObj;
	Ptr<SIFT> detectorSiftObj;
	Ptr<AKAZE> detectorAkazeObj;
	Ptr<KAZE> detectorKazeObj;

	if(surf)
		detectorSurfObj = SURF::create(minHessian);
	else if(sift)		
		detectorSiftObj = SIFT::create(minHessian);
	else if(akaze)		
		detectorAkazeObj = AKAZE::create();
	else if(kaze)	//Not working	
		detectorKazeObj = KAZE::create();

  	vector<KeyPoint> keyPointsObj;
	Mat descriptorObj;

	if (surf)
  		detectorSurfObj->detectAndCompute(imgObj, noArray(), keyPointsObj, descriptorObj);
	else if(sift)
		detectorSiftObj->detectAndCompute(imgObj, noArray(), keyPointsObj, descriptorObj);
	else if(akaze)
		detectorAkazeObj->detectAndCompute(imgObj, noArray(), keyPointsObj, descriptorObj);
	else if(kaze)	//Not working
		detectorKazeObj->detectAndCompute(imgObj, noArray(), keyPointsObj, descriptorObj);


  	//-- Draw keypoints
  	Mat imgObjKeyPoints;

	drawKeypoints(imgObj, keyPointsObj, imgObjKeyPoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT); 

	Ptr<SURF> detectorSurfFrame;
	Ptr<SIFT> detectorSiftFrame;
	Ptr<AKAZE> detectorAkazeFrame;
	Ptr<KAZE> detectorKazeFrame;

	printf("Number of key points: %i\n", (int)keyPointsObj.size());

  	//-- Show detected (drawn) keypoints
  	imshow("Keypoints used", imgObjKeyPoints);	

    for(;;) {

		try	{

			Mat frame;
	        cap >> frame; // get a new frame from camera
	
			if(surf)
				detectorSurfFrame = SURF::create(minHessian);
			else if(sift)		
				detectorSiftFrame = SIFT::create(minHessian);
			else if(akaze)		
				detectorAkazeFrame = AKAZE::create();
			else if(kaze)		
				detectorKazeFrame = KAZE::create();

			vector<KeyPoint> keyPointsFrame;
			Mat descriptorFrame;

			if(surf)
	  			detectorSurfFrame->detectAndCompute(frame, noArray(), keyPointsFrame, descriptorFrame);
			else if(sift)
				detectorSiftFrame->detectAndCompute(frame, noArray(), keyPointsFrame, descriptorFrame);
			else if(akaze)
				detectorAkazeFrame->detectAndCompute(frame, noArray(), keyPointsFrame, descriptorFrame);
			else if(kaze)
				detectorKazeFrame->detectAndCompute(frame, noArray(), keyPointsFrame, descriptorFrame);

	  		//-- Draw keypoints
	  		Mat imgFrameKeyPoints;

	  		drawKeypoints(frame, keyPointsFrame, imgFrameKeyPoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

			if(surf || sift) {
				//-- Step 2: Matching descriptor vectors with a FLANN based matcher
	    		// Since SURF and SIFT is a floating-point descriptor NORM_L2 is used
	    		matcherFLANN = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
			}
	
	    	vector< std::vector<DMatch> > knnMatches;

			if(surf || sift)
	    		matcherFLANN->knnMatch(descriptorObj, descriptorFrame, knnMatches, 2);
			else
				matcherBF.knnMatch(descriptorObj, descriptorFrame, knnMatches, 2);

			//-- Filter matches using the Lowe's ratio test
	    	const float ratio_thresh = 0.75f;
	    	vector<DMatch> goodMatches;
	    	for (size_t i = 0; i < knnMatches.size(); i++) {

	    	    if (knnMatches[i][0].distance < ratio_thresh * knnMatches[i][1].distance)
	    	        goodMatches.push_back(knnMatches[i][0]);
	    	   
	    	}    	

			Mat imgMatches;

			if(goodMatches.size() >= keyPointsObj.size() * 0.04) {

		
				//-- Draw matches    		
				drawMatches(imgObj, keyPointsObj, frame, keyPointsFrame, goodMatches, imgMatches, Scalar::all(-1),
                	 	Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
		
				//-- Localize the object
    			std::vector<Point2f> obj;
    			std::vector<Point2f> scene;

    			for( size_t i = 0; i < goodMatches.size(); i++ ) {
        			//-- Get the keypoints from the good matches
        			obj.push_back( keyPointsObj[ goodMatches[i].queryIdx ].pt );
        			scene.push_back( keyPointsFrame[ goodMatches[i].trainIdx ].pt );
    			}
    	
				Mat H = findHomography(obj, scene, RANSAC);

	    		//-- Get the corners from the image_1 ( the object to be "detected" )
	    		std::vector<Point2f> objCorners(4);
	    		objCorners[0] = Point2f(0, 0);
	    		objCorners[1] = Point2f( (float)imgObj.cols, 0 );
	    		objCorners[2] = Point2f( (float)imgObj.cols, (float)imgObj.rows );
	    		objCorners[3] = Point2f( 0, (float)imgObj.rows );

	    		std::vector<Point2f> sceneCorners(4);

				if(!(H.empty())) {

	    			perspectiveTransform(objCorners, sceneCorners, H);

	    			//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	    			line(imgMatches, sceneCorners[0] + Point2f((float)imgObj.cols, 0),
	    	    		  sceneCorners[1] + Point2f((float)imgObj.cols, 0), Scalar(0, 255, 0), 4 );
	    			line(imgMatches, sceneCorners[1] + Point2f((float)imgObj.cols, 0),
	    	    		  sceneCorners[2] + Point2f((float)imgObj.cols, 0), Scalar( 0, 255, 0), 4 );
	    			line(imgMatches, sceneCorners[2] + Point2f((float)imgObj.cols, 0),
	    	    		  sceneCorners[3] + Point2f((float)imgObj.cols, 0), Scalar( 0, 255, 0), 4 );
	    			line(imgMatches, sceneCorners[3] + Point2f((float)imgObj.cols, 0),
	    	    		  sceneCorners[0] + Point2f((float)imgObj.cols, 0), Scalar( 0, 255, 0), 4 );

				}//if(!(H.empty())) ends

			}//if(goodMatches.size() >= 4)
			else
				imgMatches = frame;

    		//-- Show detected matches
    		imshow("Proyecto01: Puntos de interes", imgMatches);

        	//cvtColor(frame, edges, COLOR_BGR2GRAY);
        	//GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        	//Canny(edges, edges, 0, 30, 3);

			char key = waitKey(10);

        	//imshow("Proyecto01: Puntos de interes", frame);
        	if(key == 113 || key == 81 ) {
				printf("Leaving loop!\n");
				break;

			}

		}//try ends
		catch(int e)
		{//catch begins

			printf("Exception number: %i\n", e);

		}//catch ends

    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
