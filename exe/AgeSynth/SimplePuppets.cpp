///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012, Tadas Baltrusaitis, all rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//     * The software is provided under the terms of this licence stricly for
//       academic, non-commercial, not-for-profit purposes.
//     * Redistributions of source code must retain the above copyright notice, 
//       this list of conditions (licence) and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright 
//       notice, this list of conditions (licence) and the following disclaimer 
//       in the documentation and/or other materials provided with the 
//       distribution.
//     * The name of the author may not be used to endorse or promote products 
//       derived from this software without specific prior written permission.
//     * As this software depends on other libraries, the user must adhere to 
//       and keep in place any licencing terms of those libraries.
//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite the following work:
//
//       Tadas Baltrusaitis, Peter Robinson, and Louis-Philippe Morency. 3D
//       Constrained Local Model for Rigid and Non-Rigid Facial Tracking.
//       IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 2012.    
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
// EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////


// SimpleCLM.cpp : Defines the entry point for the console application.

#include "SimplePuppets.h"
#include <omp.h>
#include <filesystem.hpp>
#include <filesystem\fstream.hpp>


omp_lock_t writelock;


// The modules that are being used for tracking
CLMTracker::TrackerCLM clmModel;
//A few OpenCV mats:
Mat faceimg;				//face
Mat avatarWarpedHead2;		//warped face
Mat avatarS2;				//shape
String avatarfile2;			//file where the avatar is
string file = "..\\videos\\default.wmv";
string oldfile;

bool GETFACE = false;		//get a new face
bool SHOWIMAGE = false;		//show the webcam image (in the main window). Turned on when the matrix isn't empty







void readFromStock(int c ){


	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), 0);
	USEWEBCAM = 0;
	resetERIExpression();
	CHANGESOURCE = true;
	NEWFILE = true;


	if(c == 0)
		inputfile = "..\\videos\\dreamy3.avi";
	if(c == 1)
		inputfile = "..\\videos\\afraid0.avi";
	if(c == 2)
		inputfile = "..\\videos\\comfortable0.avi";
	if(c == 3)
		inputfile = "..\\videos\\disliking2.avi";
	if(c == 4)
		inputfile = "..\\videos\\shocked2.avi";
	if(c == 5)
		inputfile = "..\\videos\\sad3.avi";
	if(c == 6)
		inputfile = "..\\videos\\heartbroken2.avi";
	if(c == 7)
		inputfile = "..\\videos\\angry1.avi";
	if(c == 8)
		inputfile = "..\\videos\\inspired11.avi";





	cout << "Loading Video File..... " << endl;
	g_print ("%s\n", inputfile);


}


void ageing(Mat faceImg)
{
	double t = (double)getTickCount();
	Mat resizedFace;

	resize(faceImg, resizedFace, Size(148, 151), 0, 0, INTER_LINEAR);

	Mat tex = am.getAppModel().imageToTexture(resizedFace);
	Mat test = am.getAppModel().textureToImage(tex);
			
	Mat appParams = am.getAppModel().fitImageToAppModel(tex);

	// get the target age from the slider
	int targetAge = int(gtk_adjustment_get_value(gtk_range_get_adjustment( GTK_RANGE(ageScale))));

	// get the gender from the dropdown
	int g = int(gtk_combo_box_get_active(GTK_COMBO_BOX(genderChoice)));
	char gender;
	switch(g)
	{
	case 0:
		gender = 'm';
		break;
	case 1:
		gender = 'f';
		break;
	default:
		gender = 'm';
	}

	Mat agedParams = am.changeFaceAge(appParams, targetAge, gender);

	Mat agedTex = am.getAppModel().appParamsToTexture(agedParams);
	//imshow("aged image", am.getAppModel().textureToImage(agedTex));

	// use the aged face as the avatar
	avatarWarpedHead2 = am.getAppModel().textureToImage(agedTex);
	
	// Needed to make sure that when pose changes the program doesn't crash
	setWriteAvatar(true);

	cout << "Time to age: " << ((double)getTickCount() - t)/getTickFrequency() << endl;

}




void Puppets(){		//this is where the magic happens! Sort of. 



	if(!clmModel._clm._plocal.empty()){		//Just in case there's nothing there...

		Mat localShape;
		Mat newshape;
		Mat globalShape;

		// Set to defaults - don't have ability to change in this
		double mouth = 100;
		double eyebrows = 100;
		double smile = 100;		//weight of expression parameters

		double headmovement = 1;

		Mat oldshape;
		clmModel._clm._plocal.copyTo(localShape);
		clmModel._clm._pglobl.copyTo(globalShape);

		clmModel._clm._pdm.CalcShape2D(oldshape, localShape, globalShape);		//calculate old shape

		clmModel._clm._pdm.CalcShape2D(newshape, localShape, globalShape); //calculate new shape

		ParseToPAW(newshape, localShape, globalShape);
		Mat teethimg;
		cvtColor(faceimg, teethimg, CV_RGB2BGR);
		cv::Mat neutralshape(newshape.rows, 1, CV_64FC1);

		Vec3d orientation;
		orientation(0) = globalShape.at<double>(1);
		orientation(1) = globalShape.at<double>(2);
		orientation(2) = globalShape.at<double>(3);

		int viewid = clmModel._det_valid.GetViewId(orientation);

		bool toggleERI = ERIon;

		if(viewid != 0){
			toggleERI = 1;
		}
		

		clmModel._det_valid._fcheck[viewid]._paw.WarpToNeutral(faceimg, newshape, neutralshape);		//warp to neutral (in CLM/PAW.cc)


		// If the first frame then do ageing and set the aged face as the avatar
		if (firstFrame)
		{
			firstFrame = false;
			
			ageing(faceimg);
		}
		//imshow("Avatar", avatarWarpedHead2);
		//imshow("Original", faceimg);

		//***************//	
		if(avatarWarpedHead2.empty() || PAWREADNEXTTIME){
			ageing(faceimg);
		}

		if (ageFace)
		{
			ageFace = false;
			ageing(faceimg);
		}


		float downratio = 1.0; 

		if(teethimg.rows > teethimg.cols){
			downratio = float(teethimg.rows) / 512.0;
			float aspect = float(teethimg.rows) / float(teethimg.cols);
			resize(teethimg,teethimg,Size(512/aspect,512),0,0,INTER_NEAREST  );				//teethimg is actually the whole background image, used for the teeth only if facereplacement is off
		}
		else{	
			downratio = float(teethimg.cols) / 512.0;
			float aspect = float(teethimg.cols) / float(teethimg.rows);
			resize(teethimg,teethimg,Size(512,512/aspect),0,0,INTER_NEAREST );				
		}



		for(int k = 0 ; k < newshape.rows ; k ++){
			newshape.at<double>(k,0) /= downratio;		
			oldshape.at<double>(k,0) /= downratio;	
		}

		sendOptions(writeToFile,toggleERI, choiceavatar);	//send writeto and usesave to avatar

		DoOpenglStuff(teethimg, oldshape, newshape, neutralshape, clmModel._det_valid._fcheck[0]._paw._tri, localShape, faceimg, avatarWarpedHead2, viewid, 1);
		//***************//



		//if(PAWREADNEXTTIME){
		//	ageing(faceimg);
		//}




	}

}



void use_webcam(){			//called when the 'use webcam' checkbox is ticked
	USEWEBCAM = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
	CHANGESOURCE = true;
	resetERIExpression();
	if(USEWEBCAM){
		cout << "Using Webcam. " << endl;
	}
	else{
		cout << "Not using Webcam. " << endl;
	}
	resetERIExpression();
}


static gboolean time_handler( GtkWidget *widget ) {
	return TRUE;
}

gboolean expose_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{


	if(SHOWIMAGE){
		pix = gdk_pixbuf_new_from_data( (guchar*)opencvImage.imageData,
			GDK_COLORSPACE_RGB, FALSE, opencvImage.depth, opencvImage.width,
			opencvImage.height, (opencvImage.widthStep), NULL, NULL);

		gdk_draw_pixbuf(widget->window,
			widget->style->fg_gc[GTK_WIDGET_STATE (widget)], pix, 0, 0, 0, 0,
			opencvImage.width, opencvImage.height, GDK_RGB_DITHER_NONE, 0, 0); /* Other possible values are  GDK_RGB_DITHER_MAX,  GDK_RGB_DITHER_NORMAL */
	}



	return TRUE;
}


static void file_ok_sel_z( GtkWidget        *w,
	GtkFileSelection *fs )
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), 0);
	USEWEBCAM = 0;
	CHANGESOURCE = true;
	resetERIExpression();
	CHANGESOURCE = true;
	NEWFILE = true;
	inputfile = gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs));
	cout << "Loading Video File..... " << endl;
	g_print ("%s\n", inputfile);
	gtk_widget_destroy(filez);
}


/* Our callback.
* The data passed to this function is printed to stdout */
static void callback( GtkWidget *widget,
	gpointer   data )
{
	g_print ("Hello again - %s was pressed\n", (char *) data);

	if((char *) data=="load video"){

		/* Create a new file selection widget */
		filez = gtk_file_selection_new ("File selection");

		//g_signal_connect (filez, "destroy",
		//			  G_CALLBACK (gtk_main_quit), NULL);
		/* Connect the ok_button to file_ok_sel_z function */
		g_signal_connect (GTK_FILE_SELECTION (filez)->ok_button,
			"clicked", G_CALLBACK (file_ok_sel_z), (gpointer) filez);

		/* Connect the cancel_button to destroy the widget */
		g_signal_connect_swapped (GTK_FILE_SELECTION (filez)->cancel_button,
			"clicked", G_CALLBACK (gtk_widget_destroy),
			filez);

		/* Lets set the filename, as if this were a save dialog, and we are giving
		a default filename */
		gtk_file_selection_set_filename (GTK_FILE_SELECTION(filez), "..\\videos\\");

		gtk_widget_show (filez);
	}

	// If face changed then need to age face
	if((char *) data=="face changed")
		ageFace = true;

}

/* This callback quits the program */
static gboolean delete_event( GtkWidget *widget,
	GdkEvent  *event,
	gpointer   data )
{
	quitmain=true;
	return 0;
}

// Called when any ageing choice is changed, just sets ageFace to true so face aged next time
static void ageingChoicesChanged(GtkWidget *widget, gpointer data)
{
	ageFace = true;
}



static void printErrorAndAbort( const std::string & error )
{
	std::cout << error << std::endl;
	abort();
}

#define FATAL_STREAM( stream ) \
	printErrorAndAbort( std::string( "Fatal error: " ) + stream )

using namespace std;
using namespace cv;

// takes in doubles for orientation for added precision, but ultimately returns a float matrix
Matx33f Euler2RotationMatrix(const Vec3d& eulerAngles)
{
	Matx33f rotationMatrix;

	double s1 = sin(eulerAngles[0]);
	double s2 = sin(eulerAngles[1]);
	double s3 = sin(eulerAngles[2]);

	double c1 = cos(eulerAngles[0]);
	double c2 = cos(eulerAngles[1]);
	double c3 = cos(eulerAngles[2]);

	rotationMatrix(0,0) = (float)(c2 * c3);
	rotationMatrix(0,1) = (float)(-c2 *s3);
	rotationMatrix(0,2) = (float)(s2);
	rotationMatrix(1,0) = (float)(c1 * s3 + c3 * s1 * s2);
	rotationMatrix(1,1) = (float)(c1 * c3 - s1 * s2 * s3);
	rotationMatrix(1,2) = (float)(-c2 * s1);
	rotationMatrix(2,0) = (float)(s1 * s3 - c1 * c3 * s2);
	rotationMatrix(2,1) = (float)(c3 * s1 + c1 * s2 * s3);
	rotationMatrix(2,2) = (float)(c1 * c2);

	return rotationMatrix;
}

void Project(Mat_<float>& dest, const Mat_<float>& mesh, Size size, double fx, double fy, double cx, double cy)
{
	dest = Mat_<float>(mesh.rows,2, 0.0);

	int NbPoints = mesh.rows;

	register float X, Y, Z;


	Mat_<float>::const_iterator mData = mesh.begin();
	Mat_<float>::iterator projected = dest.begin();

	for(int i = 0;i < NbPoints; i++)
	{
		// Get the points
		X = *(mData++);
		Y = *(mData++);
		Z = *(mData++);

		float x;
		float y;

		// if depth is 0 the projection is different
		if(Z != 0)
		{
			x = (float)((X * fx / Z) + cx);
			y = (float)((Y * fy / Z) + cy);
		}
		else
		{
			x = X;
			y = Y;
		}

		// Clamping to image size
		if( x < 0 )	
		{
			x = 0.0;
		}
		else if (x > size.width - 1)
		{
			x = size.width - 1.0f;
		}
		if( y < 0 )
		{
			y = 0.0;
		}
		else if( y > size.height - 1) 
		{
			y = size.height - 1.0f;
		}

		// Project and store in dest matrix
		(*projected++) = x;
		(*projected++) = y;
	}

}

// Move all of this to OpenGL?
void DrawBox(Mat image, Vec6d pose, Scalar color, int thickness, float fx, float fy, float cx, float cy)
{
	float boxVerts[] = {-1, 1, -1,
		1, 1, -1,
		1, 1, 1,
		-1, 1, 1,
		1, -1, 1,
		1, -1, -1,
		-1, -1, -1,
		-1, -1, 1};
	Mat_<float> box = Mat(8, 3, CV_32F, boxVerts).clone() * 100;


	Matx33f rot = Euler2RotationMatrix(Vec3d(pose[3], pose[4], pose[5]));
	Mat_<float> rotBox;

	Mat((Mat(rot) * box.t())).copyTo(rotBox);
	rotBox = rotBox.t();

	rotBox.col(0) = rotBox.col(0) + pose[0];
	rotBox.col(1) = rotBox.col(1) + pose[1];
	rotBox.col(2) = rotBox.col(2) + pose[2];

	// draw the lines
	Mat_<float> rotBoxProj;
	Project(rotBoxProj, rotBox, image.size(), fx, fy, cx, cy);

	Mat begin;
	Mat end;

	rotBoxProj.row(0).copyTo(begin);
	rotBoxProj.row(1).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(1).copyTo(begin);
	rotBoxProj.row(2).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(2).copyTo(begin);
	rotBoxProj.row(3).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(0).copyTo(begin);
	rotBoxProj.row(3).copyTo(end);
	//std::cout << begin <<endl;
	//std::cout << end <<endl;
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(2).copyTo(begin);
	rotBoxProj.row(4).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(1).copyTo(begin);
	rotBoxProj.row(5).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(0).copyTo(begin);
	rotBoxProj.row(6).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(3).copyTo(begin);
	rotBoxProj.row(7).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(6).copyTo(begin);
	rotBoxProj.row(5).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(5).copyTo(begin);
	rotBoxProj.row(4).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(4).copyTo(begin);
	rotBoxProj.row(7).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);

	rotBoxProj.row(7).copyTo(begin);
	rotBoxProj.row(6).copyTo(end);
	cv::line(image, Point((int)begin.at<float>(0), (int)begin.at<float>(1)), Point((int)end.at<float>(0), (int)end.at<float>(1)), color, thickness);


}

vector<string> get_arguments(int argc, char **argv)
{

	vector<string> arguments;

	for(int i = 1; i < argc; ++i)
	{
		arguments.push_back(string(argv[i]));
	}
	return arguments;
}



void doFaceTracking(int argc, char **argv){


	bool done = false;

	while(!done )
	{

		while(gtk_events_pending ()){
			gtk_main_iteration ();
		}


		cout << gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check)) << endl;



		cout << "Not done yet!" << endl;
		cout << USEWEBCAM << ", " << NEWFILE << endl;


		vector<string> arguments = get_arguments(argc, argv);

		// Some initial parameters that can be overriden from command line	
		vector<string> files, dDirs, outposes, outvideos, outfeatures;

		// By default try webcam
		int device = 0;

		// cx and cy aren't always half dimx or half dimy, so need to be able to override it (start with unit vals and init them if none specified)
		float fx = 500, fy = 500, cx = 0, cy = 0;
		int dimx = 0, dimy = 0;

		bool useCLMTracker = true;

		CLMWrapper::CLMParameters clmParams(arguments);

		clmParams.wSizeCurrent = clmParams.wSizeInit;

		PoseDetectorHaar::PoseDetectorHaarParameters haarParams;

#if OS_UNIX
		haarParams.ClassifierLocation = "/usr/share/OpenCV-2.3.1/haarcascades/haarcascade_frontalface_alt.xml";
#else
		haarParams.ClassifierLocation = "..\\lib\\3rdParty\\OpenCV\\classifiers\\haarcascade_frontalface_alt.xml";
#endif

		// Get the input output file parameters
		CLMWrapper::get_video_input_output_params(files, dDirs, outposes, outvideos, outfeatures, arguments);
		// Get camera parameters
		CLMWrapper::get_camera_params(fx, fy, cx, cy, dimx, dimy, arguments);    


		// Face detector initialisation
		CascadeClassifier classifier(haarParams.ClassifierLocation);


		int f_n = -1;

		// We might specify multiple video files as arguments
		if(files.size() > 0)
		{
			f_n++;			
			file = files[f_n];
		}

		if(NEWFILE){
			file = inputfile;
		}

		bool readDepth = !dDirs.empty();	




		/*			if(SOURCEAVATAR){
		cout << "Trying to load from file: " << endl;
		USEWEBCAM = false;
		file = FaceFileName;
		writeToFile = !writeToFile;
		PAWREADAGAIN = true;
		quitmain = true;
		}
		*/

		if(USEWEBCAM)
		{
			INFO_STREAM( "Attempting to capture from device: " << device );
			vCap = VideoCapture( device );
			if( !vCap.isOpened() ) {
				GtkWidget *dialog;
				dialog = gtk_message_dialog_new (GTK_WINDOW (window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					"Error loading Webcam. Please load video file");

				/* Destroy the dialog when the user responds to it (e.g. clicks a button) */
				g_signal_connect_swapped (dialog, "response",
					G_CALLBACK (gtk_widget_destroy),
					dialog);
				gtk_widget_show (dialog);

				USEWEBCAM = false;
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), 0);
				CHANGESOURCE = true;
				resetERIExpression();
				cout << "Not using Webcam. " << endl;
				//FATAL_STREAM( "Failed to open video source" );
			}
		}

		// Do some grabbing
		if( !USEWEBCAM)
		{

			if(file.size() > 0 ){
				INFO_STREAM( "Attempting to read from file: " << file );
				vCap = VideoCapture( file );
			}
			else {
				INFO_STREAM("No file specified. Please use webcam or load file manually");
				USEWEBCAM = 1;

			}
		}



		Mat img;
		vCap >> img;

		if (CHANGESOURCE)
			ageFace = true;  // Need to do face ageing if source is changed


		// If no dimensions defined, do not do any resizing
		if(dimx == 0 || dimy == 0)
		{
			dimx = img.cols;
			dimy = img.rows;
		}

		// If optical centers are not defined just use center of image
		if(cx == 0 || cy == 0)
		{
			cx = dimx / 2.0f;
			cy = dimy / 2.0f;
		}

		//for constant-size input:
		//dimx = 200;
		//dimy = 200;
		//cx = 100;
		//cy = 100;



		int frameProc = 0;

		// faces in a row detected
		facesInRow = 0;

		// saving the videos
		VideoWriter writerFace;
		if(!outvideos.empty())
		{
			writerFace = VideoWriter(outvideos[f_n], CV_FOURCC('D','I','V','X'), 30, img.size(), true);		
		}

		// Variables useful for the tracking itself
		bool success = false;
		trackingInitialised = false;

		// For measuring the timings
		int64 t1,t0 = cv::getTickCount();
		double fps = 10;

		Mat disp;
		Mat rgbimg;

		CHANGESOURCE = false;


		//todo: fix bug with crash when selecting video file to play under webcam mode (disable video select button?)
		//also occasionally opencv error when changing between different sizes of video input/webcam owing to shape going outside boundries. 



		while(!img.empty() && !CHANGESOURCE && !done)						//This is where stuff happens once the file's open.
		{		


			//for constant-size input:
			//resize(img, img, Size( dimx, dimy));

			Mat_<float> depth;
			Mat_<uchar> gray;
			cvtColor(img, gray, CV_BGR2GRAY);
			cvtColor(img, rgbimg, CV_BGR2RGB);

			if(GRAYSCALE){
				cvtColor(gray, rgbimg, CV_GRAY2RGB);
			}



			parsecolour(rgbimg);			//this sends the rgb image to the PAW loop

			writeToFile = 0;


			if(GETFACE){
				GETFACE = false;
				writeToFile = !writeToFile;
				PAWREADAGAIN = true;
			}



			// Don't resize if it's unneeded
			Mat_<uchar> img_scaled;		
			if(dimx != gray.cols || dimy != gray.rows)
			{
				resize( gray, img_scaled, Size( dimx, dimy ) , 0, 0, INTER_NEAREST );
				resize(img, disp, Size( dimx, dimy), 0, 0, INTER_NEAREST );
			}
			else
			{
				img_scaled = gray;
				disp = img.clone();
			}


			disp.copyTo(faceimg);



			//namedWindow("colour",1);

			// Get depth image
			if(readDepth)
			{
				char* dst = new char[100];
				std::stringstream sstream;
				//sstream << dDir << "\\depth%06d.png";
				sstream << dDirs[f_n] << "\\depth%05d.png";
				sprintf(dst, sstream.str().c_str(), frameProc + 1);
				Mat_<short> dImg = imread(string(dst), -1);
				if(!dImg.empty())
				{
					if(dimx != dImg.cols || dimy != dImg.rows)
					{
						Mat_<short> dImgT;
						resize(dImg, dImgT, Size( dimx, dimy), 0, 0, INTER_NEAREST );
						dImgT.convertTo(depth, CV_32F);
					}
					else
					{
						dImg.convertTo(depth, CV_32F);
					}
				}
				else
				{
					WARN_STREAM( "Can't find depth image" );
				}
			}

			Vec6d poseEstimateHaar;
			Matx66d poseEstimateHaarUncertainty;

			Rect faceRegion;



			// The start place where CLM should start a search (or if it fails, can use the frame detection)
			if(!trackingInitialised || (!success && ( frameProc  % 5 == 0)))
			{
				// The tracker can return multiple head pose observation
				vector<Vec6d> poseEstimatesInitialiser;
				vector<Matx66d> covariancesInitialiser;			
				vector<Rect> regionsInitialiser;

				bool initSuccess = PoseDetectorHaar::InitialisePosesHaar(img_scaled, depth, poseEstimatesInitialiser, covariancesInitialiser, regionsInitialiser, classifier, fx, fy, cx, cy, haarParams);

				if(initSuccess)
				{
					if(poseEstimatesInitialiser.size() > 1)
					{
						cout << "ambiguous detection ";
						// keep the closest one (this is a hack for the experiment)
						double best = 10000;
						int bestIndex = -1;
						for( size_t i = 0; i < poseEstimatesInitialiser.size(); ++i)
						{
							cout << poseEstimatesInitialiser[i][2] << " ";
							if(poseEstimatesInitialiser[i][2] < best  && poseEstimatesInitialiser[i][2] > 200)
							{
								bestIndex = i;
								best = poseEstimatesInitialiser[i][2];
							}									
						}
						if(bestIndex != -1)
						{
							cout << endl << "Choosing " << poseEstimatesInitialiser[bestIndex][2] << regionsInitialiser[bestIndex].x << " " << regionsInitialiser[bestIndex].y <<  " " << regionsInitialiser[bestIndex].width << " " <<  regionsInitialiser[bestIndex].height << endl;
							faceRegion = regionsInitialiser[bestIndex];
						}
						else
						{
							initSuccess = false;
						}
					}
					else
					{	
						faceRegion = regionsInitialiser[0];
					}				

					facesInRow++;
				}
			}






			// If condition for tracking is met initialise the trackers
			if(!trackingInitialised && facesInRow >= 1)
			{			
				trackingInitialised = CLMWrapper::InitialiseCLM(img_scaled, depth, clmModel, poseEstimateHaar, faceRegion, fx, fy, cx, cy, clmParams);		
				facesInRow = 0;
			}		

			// opencv detector is needed here, if tracking failed reinitialise using it
			if(trackingInitialised)
			{
				success = CLMWrapper::TrackCLM(img_scaled, depth, clmModel, vector<Vec6d>(), vector<Matx66d>(), faceRegion, fx, fy, cx, cy, clmParams);								
			}			
			if(success)
			{			
				clmParams.wSizeCurrent = clmParams.wSizeSmall;
			}
			else
			{
				clmParams.wSizeCurrent = clmParams.wSizeInit;
			}

			// Changes for no reinit version
			//success = true;
			//clmParams.wSizeCurrent = clmParams.wSizeInit;

			Vec6d poseEstimateCLM = CLMWrapper::GetPoseCLM(clmModel, fx, fy, cx, cy, clmParams);



			if(success)			
			{
				int idx = clmModel._clm.GetViewIdx(); 	

				// drawing the facial features on the face if tracking is successful
				clmModel._clm._pdm.Draw(disp, clmModel._shape, clmModel._clm._triangulations[idx], clmModel._clm._visi[0][idx]);


				//cout << clmModel._clm.shape << endl;
				//cv::imshow("other", clmModel._shape);

				DrawBox(disp, poseEstimateCLM, Scalar(255,0,0), 3, fx, fy, cx, cy);			
			}
			else if(!clmModel._clm._pglobl.empty())
			{			
				int idx = clmModel._clm.GetViewIdx(); 	

				// draw the facial features
				clmModel._clm._pdm.Draw(disp, clmModel._shape, clmModel._clm._triangulations[idx], clmModel._clm._visi[0][idx]);

				// if tracking fails draw a red outline
				DrawBox(disp, poseEstimateCLM, Scalar(0,0,255), 3, fx, fy, cx, cy);	
			}









			if(frameProc % 10 == 0)
			{      
				t1 = cv::getTickCount();
				fps = 10.0 / (double(t1-t0)/cv::getTickFrequency()); 
				t0 = t1;
			}

			frameProc++;
			Mat disprgb;			
			//imshow("colour", disp);

			cvtColor(disp, disprgb, CV_RGB2BGR);
			resize(disprgb,disprgb,Size(500,400),0,0,INTER_NEAREST );


			char fpsC[255];
			_itoa((int)fps, fpsC, 10);
			string fpsSt("FPS:");
			fpsSt += fpsC;
			cv::putText(disprgb, fpsSt, cv::Point(10,20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255,0,0));



			opencvImage = disprgb;

			if(disprgb.empty()){
				SHOWIMAGE = false;
			}
			else{
				SHOWIMAGE = true;
			}

			gtk_widget_draw(GTK_WIDGET(drawing_area), NULL);

			if(!depth.empty())
			{
				imshow("depth", depth/2000.0);
			}

			vCap >> img;

			if(!outvideos.empty())
			{		
				writerFace << disp;
			}

			// detect key presses
			char c = cv::waitKey(1);

			// key detections







			sendERIstrength(33);








			if(int(gtk_combo_box_get_active(GTK_COMBO_BOX(inputchoice))) != mindreadervideo){

				mindreadervideo = int(gtk_combo_box_get_active(GTK_COMBO_BOX(inputchoice)));
				cout << "Choice: " << mindreadervideo << endl;

				readFromStock(mindreadervideo);

			}

			
			GRAYSCALE = false;

		

			while(gtk_events_pending ()){
				gtk_main_iteration ();
			}




			//if(PAWREADNEXTTIME){
			//	sendAvatarFile(avatarfile2);
			//	cout << "Read Again!" << endl;
			//	PAWREADNEXTTIME = false;
			//	cout << "PAW READ NEXT TIME OFF" << endl;
			//}
			if(PAWREADAGAIN){
				//avatarfile2 = string("../images/shape_central" + choiceavatar + ".yml");
				firstFrame = true;
				PAWREADNEXTTIME = true;
				PAWREADAGAIN = false;
				cout << "PAW READ NEXT TIME ON" << endl;
			}




			Puppets();





			if(quitmain==1){
				cout << "Quit." << endl;
				return;
			}







			// restart the tracker
			if(c == 'r')
			{
				trackingInitialised = false;
				facesInRow = 0;
			}





		}			





		trackingInitialised = false;
		facesInRow = 0;

		/*// break out of the loop if done with all the files
		if(f_n == files.size() -1)
		{


		//	if( waitKey( 5000 ) >= 0 )
		//    			done = true;

		}*/





	}



}

void startGTK(int argc, char **argv){



	gtk_init (&argc, &argv);

	/* Create a new window */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);



	bool USEGLADE = false;			//import the .xml window designed using the GLADE interface

	if(USEGLADE){

		GtkBuilder *builder;
		builder = gtk_builder_new ();
		gtk_builder_add_from_file (builder, "../gtkmainscreen.glade", NULL);
		testwindow = GTK_WIDGET(gtk_builder_get_object (builder, "window1"));
		    GtkWidget* exitButton = 0;
   exitButton =  GTK_WIDGET(gtk_builder_get_object (builder, "exit"));
   	g_signal_connect (exitButton, "clicked",
		G_CALLBACK (delete_event), NULL);
		gtk_builder_connect_signals (builder, NULL);          
		g_object_unref (G_OBJECT (builder));

		gtk_widget_show (testwindow);

	}






	/* Set the window title */
	gtk_window_set_title (GTK_WINDOW (window), "Age Synthesis Control Panel");

	/* Set a handler for delete_event that immediately
	* exits GTK. */
	g_signal_connect (window, "delete-event",
		G_CALLBACK (delete_event), NULL);

	/* Sets the border width of the window. */
	gtk_container_set_border_width (GTK_CONTAINER (window), 20);

	/* Create an n x m table */
	table = gtk_table_new(15,5,TRUE);

	/* Put the table in the main window */
	gtk_container_add (GTK_CONTAINER (window), table);

	

	/* Create "Quit" button */
	button = gtk_button_new_with_label ("Quit");


	/* When the button is clicked, we call the "delete-event" function
	* and the program exits */
	g_signal_connect (button, "clicked",
		G_CALLBACK (delete_event), NULL);



	
	/* Create button: load video file */

	button = gtk_button_new_with_label ("Load Video File");

	/* When the button is clicked, we call the "callback" function
	* with a pointer to "load video" as its argument */
	g_signal_connect (button, "clicked",
		G_CALLBACK (callback), (gpointer) "load video");
	/* Insert button into the table */
	gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 1, 2, 3);

	gtk_widget_show (button);


	// Create button: face changed
	button = gtk_button_new_with_label ("Face Changed");
	// Connect the button to call back for click event
	g_signal_connect (button, "clicked",
		G_CALLBACK (callback), (gpointer) "face changed");
	/* Insert button into the table */
	gtk_table_attach_defaults (GTK_TABLE (table), button, 1, 2, 8, 9);

	gtk_widget_show (button);

	/* Add a check button to select the webcam by default */
	check = gtk_check_button_new_with_label ("Use Webcam");
	gtk_signal_connect(GTK_OBJECT (check), "pressed",use_webcam, NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), 0);
	gtk_table_attach_defaults (GTK_TABLE (table), check, 0, 1, 4,5);
	gtk_widget_show(check);


	/* Create "Quit" button */
	button = gtk_button_new_with_label ("Quit");


	/* When the button is clicked, we call the "delete-event" function
	* and the program exits */
	g_signal_connect (button, "clicked",
		G_CALLBACK (delete_event), NULL);




	/* Now add a child widget to the aspect frame */
	drawing_area = gtk_drawing_area_new();

	/* Ask for a window   */
	gtk_widget_set_size_request(drawing_area, opencvImage.width,opencvImage.height);
	gtk_table_attach_defaults (GTK_TABLE (table), drawing_area, 3, 5, 0, 15);

	g_signal_connect(G_OBJECT (drawing_area), "expose_event", G_CALLBACK (expose_event_callback), NULL);
	gtk_widget_show(drawing_area);

	time_handler(window);

	// Add the age adjustment slider
	adjAge = gtk_adjustment_new(30, 20, 100, 1, 5, 5);

	ageScale = gtk_hscale_new (GTK_ADJUSTMENT (adjAge));
	gtk_widget_set_size_request (GTK_WIDGET (ageScale), 200, -1);
	gtk_table_attach_defaults (GTK_TABLE (table), ageScale, 1, 2, 6,7);

	g_signal_connect(G_OBJECT(ageScale), "value_changed", G_CALLBACK(ageingChoicesChanged), NULL);  // Connect ageScale to function when it is changed

	gtk_range_set_update_policy( GTK_RANGE(ageScale), GTK_UPDATE_DELAYED);
	gtk_widget_show (ageScale);

	ageLabel = gtk_label_new("Target Age");
	gtk_table_attach_defaults (GTK_TABLE (table), ageLabel, 0, 1, 6, 7);
	gtk_widget_show (ageLabel);


	// Add the gender selection dropdown
	genderChoice = gtk_combo_box_entry_new_text();

	//note the combo box selections are identified by the program only by their number: 1,2,3,4, etc.
	gtk_combo_box_append_text(GTK_COMBO_BOX(genderChoice), "Male");
	gtk_combo_box_append_text(GTK_COMBO_BOX(genderChoice), "Female");

	gtk_combo_box_set_active(GTK_COMBO_BOX(genderChoice), 0);

	g_signal_connect(G_OBJECT(genderChoice), "changed", G_CALLBACK(ageingChoicesChanged), NULL);  // Connect genderChoice to function when it is changed

	gtk_table_attach_defaults (GTK_TABLE (table), genderChoice, 0, 1, 8, 9);
	gtk_widget_show(genderChoice);

	// Add the input choice (list of videos)
	inputchoice = gtk_combo_box_entry_new_text();

	//note the combo box selections are identified by the program only by their number: 1,2,3,4, etc.
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Dreamy");
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Afraid");
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Comfortable");
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Disliking");
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Shocked");
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Sad");
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Heartbroken");
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Angry");
	gtk_combo_box_append_text(GTK_COMBO_BOX(inputchoice), "Inspired");

	gtk_table_attach_defaults (GTK_TABLE (table), inputchoice, 0, 2, 10, 11);
	gtk_widget_show(inputchoice);


	/* Insert the quit button into the both 
	* lower quadrants of the table */
	gtk_table_attach_defaults (GTK_TABLE (table), button, 1, 2, 2, 3);

	gtk_widget_show (button);

	gtk_widget_show (table);
	gtk_widget_show (window);

}








int main (int argc, char **argv)
{


	startGTK(argc,argv);

	// Load the ageing model
	am = AgeingModel("AgeModel");

	omp_init_lock(&writelock);

	//todo: fix bug with crash when selecting video file to play under webcam mode (disable video select button?)
	doFaceTracking(argc,argv);
	omp_destroy_lock(&writelock);

	return 0;
}

