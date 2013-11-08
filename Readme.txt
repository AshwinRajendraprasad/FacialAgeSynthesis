The project was Was built and tested on Visual Studio 2010 (can't guarantee compatibility with 2005 and 2008).
Was tested on Windows Vista, Windows 7 and Windows 8 can't guarantee compatibility with other versions and platforms. 
Comes prepackaged with all the necessary binaries and dll's for convenience. You have to respect boost, Eigen, freeglut, glut, Glew, GTK+, TBB, and OpenCV and other licenses.
You don't need to download anything additional, just open "CLM_framework.sln" using Visual Studio 2010 and compile the code. 

--------------------------------------- Copyright information -----------------------------------------------------------------------------	

Copyright can be found in the Copyright.txt

--------------------------------------- Code Layout -----------------------------------------------------------------------------
./exe - the runner and executables that show how to use the libraries for facial expression and head pose tracking
	ConsolePuppets/ - running the face replacement demo in "Kiosk" mode
	SimpleCLM/ - running clm, clnf or clm-z if depth is supplied
	SimpleCLMImg/ - running clm or clm-z on a images, individual or in a folder
	SimpleHaar/ - running just the face detector (Viola-Jones)
	SimplePuppets/ - running the face replacement demo with more control
./lib
	3rdParty - place for 3rd party libraries
		boost - prepackaged relevant parts of the boost library
		freeglut - prepackaged freeglut library for OpenGL support, that might be used for drawing the ellipsoids or triangular meshes
		OpenCV - prepackaged OpenCV 2.4.0 library that is used extensively internally to provide support for basic computer vision functionallity
	local - the actual meat of the code where the relevant computer vision algorithms reside
		CLM - The CLM and CLM-Z algorithms
		CLMWrapper - Wrapper code making CLM calls easier
		PoseDetectorHaar - Viola-Jones detector wrapper
./matlab runners
	helper scripts for running the experiments and extracting error from them

--------------------------------------- CLM Matlab runners -----------------------------------------------------------------------------	

These are provided for recreation of some of the experiments described in the paper and to demonstrate the command line interface.
To run them you will need to change the dataset locations.

run_clm_head_pose_tests_svr.m - runs CLM, and CLM-Z on the 3 head pose datasets (Boston University, Biwi Kinect, and ICT-3DHP you need to acquire the datasets yourself)
run_clm_head_pose_tests_clnf.m - runs CLNF on the 3 head pose datasets (Boston University, Biwi Kinect, and ICT-3DHP you need to acquire the datasets yourself)
run_clm_feature_point_tests_wild.m - runs CLM and CLNF on the in the wild face datasets, using already defined bounding boxes of faces (these are produced using the 'matlab runners/ExtractBoundingBoxes.m' script on the in the wild datasets from http://ibug.doc.ic.ac.uk/resources/300-W/)

--------------------------------------- Command line parameters for video -----------------------------------------------------------------------------	

Parameters for input (if nothing is specified attempts to read from a webcam with default values)

	-f <filename> - the video file being input
	-fd <depth directory/> - the directory where depth files are stored

	optional camera parameters (used in Gavam)

	-fx <focal length in x>
	-fy <focal length in y>
	-cx <optical centre in x> 
	-cy <optical centre in y>

	-dimx <resize to dimx in x before tracking>
	-dimy <resize to dimy in y before tracking>

Parameters for output
	-op <location of output pose file>
	-of <location of output feature points file> (used in CLM code)
	-ov <location of tracked video>

Model parameters (apply to images and videos)
	-mloc <the location of CLM model>
		"model/main.txt" (default) - trained on Multi-PIE of varying pose and illumination, works well for head pose tracking
		"model/main_svr_ml.txt" - trained on Multi-PIE of varying pose and illumination, faster than above but less accurate
		"model/main_svr.txt" - trained on Multi-PIE of varying pose but single illumination
		"model/main_wild.txt" - trained on in the wild data, much more robust, but in cases less accurate
	-clm_sigma <sigma value from the RLMS and NU-RLMS algorithms, best range 1-2, will affect the fitting>
	-reg <regularisation value from the RLMS and NU-RLMS algorithms, best range 5-40, will affect the fitting, higher values will be more robust but have issues with extreme expressions>
	-multi-view <0/1>, should multi-view initialisation be used (more robust, but slower, only for images)
	
For more examples of how to run the code, please refer to the Matlab runner code which calls the compiled executables with the command line parameters.

--------------------------------------- Command line parameters for images -----------------------------------------------------------------------------	

Parameters for input (if nothing is specified attempts to read from a webcam with default values)

	-f <filename> - the image file being input

	optional camera parameters (used in Gavam)

	-fx <focal length in x>
	-fy <focal length in y>
	-cx <optical centre in x> 
	-cy <optical centre in y>

	-dimx <resize to dimx in x before tracking>
	-dimy <resize to dimy in y before tracking>

Parameters for output
	-op <location of output pose file>
	-of <location of output feature points file> (used in CLM code)
	-ov <location of tracked video>

For more examples of how to run the code, please refer to the Matlab runner code which calls the compiled executables with the command line parameters.

--------------------------------------- Depth data -----------------------------------------------------------------------------
Currently depth stream is expected to be in the format of a collection of 8 or 16-bit .png files in a folder with a naming scheme: depth00001.png, depth00002.png, ... depthxxxxx.png, with each .png.
Each depth image is expected to correspond to an intensity image from a video sequence. Colour and depth images are expected to be aligned (a pixel in one is correspondent to a pixel in another).

For example of this sort of data see the ICT-3DHP dataset (http://multicomp.ict.usc.edu/?p=1738)

--------------------------------------- CLM parameters -----------------------------------------------------------------------------	

The default parameters are already set (they work well enough on the datasets tested (BU, BU-4DFE, ICT-3DHP, and Biwi)), however,
tweaking them might results in better tracking for your specific datasets.

Main ones to tweak in lib\local\CLMWrapper\include\CLMParameters.h:

window sizes - wSizeLarge specified window sizes for each scale iteration, if set smaller the tracking accuracy might suffer but it would be faster, setting to larger area would slow down the tracking, but potentially make it more accurate.
fcheck - if set to true an SVM classifier is used to determine convergence of feature points and make sure it's a face, this is useful to know for reinitialisation
decision boundary - this is used to determine when the SVM classifier thinks that convergence failed (currently -0.5), decrease for a less conservative boundary (more false positives, but fewer false negatives) and increase to be more conservative (fewer false positives, but more false negatives)

--------------------------------------- Results -----------------------------------------------------------------------------	
	
Results that you should expect on running the code on the publicly available datasets can be found in:

Results_clm_feats.txt - the results on landmark detection on in the wild dataset
Results_clm_pose.txt - the results on landmark detection on in the wild dataset

The results are hugely affected by face detection, at the moment the code relies on OpenCV for that, hence it might not work very well in the wild if face detection is bad.

--------------------------------------- Final remarks -----------------------------------------------------------------------------	

I did my best to make sure that the code runs out of the box but there are always issues and I would be grateful for your understanding that this is research code and not a commercial
level product. However, if you encounter any probles please contact me at Tadas.Baltrusaitis@cl.cam.ac.uk for any bug reports/questions/suggestions. 
