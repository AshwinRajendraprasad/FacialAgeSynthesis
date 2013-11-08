
// OPEN GL

#define GLEW_STATIC

//freeglut is version freeglut 2.6.0-3.mp for MSVC
#include "glew.h"
#include "wglew.h"
#include "GL/freeglut.h"

#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glew32s.lib")



#include "Avatar.h"
#include <cv.h>


#include <highgui.h>
#include <imgproc.hpp>

int outputWidth, outputHeight;

VideoWriter myvideo;

Mat oldFace, oldFaceLeft, oldFaceRight, ratioFace, avatarFace,newFace, newWarpedFace, newWarpedFaceFloat,ratioFaceFloat;

bool DISPLAYHISTOGRAMS = false;			//set to 'true' to display three histograms from the CompensateColour operation
bool KIOSKMODE = false;

Mat		usedAvatar, leftAvatar, centreAvatar, rightAvatar, smileAvatar;	//These are the images of the avatar in various poses
bool	USESAVEDIMAGE = false;		//boolean variables for the Avatar control state-machine:
bool	CHANGEIMAGE = false;
bool	WRITETODISK = false;		//save current face image as an avatar
bool	resetexpression = false;
float	ERIstrength = 0.333;	//the amount of ratio image that is applied to the avatar from the warped live stream

bool FACEREPLACE = true; // if true, replaces a face on the background video. If false, projects a face onto a black background

bool UNDERLAYER = true; //if the computer is fast enough, it deals with side-lighting quite well. It blends a heavily-blurred original face below the new one

bool RECORDVIDEO = false;		//record video output (not controllable in the GUI)

GLint   windowWidth  = 512;     // Define our window width
GLint   windowHeight = 512;     // Define our window height

int FaceResolution = 128;

Mat wholeimage;
Mat alphamask;
Mat blurredmask;

float width = 512;
float height = 512;	

GLfloat fieldOfView  = 45.0f;   // FoV
GLfloat zNear        = 0.1f;    // Near clip plane
GLfloat zFar         = 256.0f;  // Far clip plane


GLuint framebuffer = 0;
GLuint renderbuffer;
GLenum status;

//textures for the various orientations of the avatar, and for the mouth fill-in:
GLuint	imageTex;	
GLuint	texleft, texright, texcentre, texmouth, texsmile, texbackground, texblurred;

string	whichavatar;			//file location of the avatar (.yml centre file)

int framesTick = 1;				//How often (every N frames) the openGL textures are re-calculated 
bool INIT = false;				

Mat scopy, scopyleft, scopycentre, scopyright, scopysmile, seyes;	//the shape matrixes for various poses
Mat camFrameCopy;

float widthavat, heightavat;		//height and width of the avatar
// Frame counting and limiting
int    frameCount = 0;

int oldposture = -5; //position of the head, 0=straight ahead, 1=right, 2=left. Initialised to a non-existant pose so 'changeposture' is called on startup

//Eyes not used as filling them in looks too weird:
Mat eyestri;
//however, these are the coordinates for the 66-point face model, for reference:
/*= (Mat_<int>(8,3) << 	36,37,41,	//first eye 14,3
37,40,41,
38,40,37,
39,38,40,
42,43,47,	//second eye
43,46,47,
43,44,46,
44,45,46);*/

//...We DO fill the oral cavity in, but instead we read the co-ordinates from lib/local/Puppets/model/mouthEyesShape.yml. 
Mat mouthtri;
//here they are again, in the 66-point face model, for reference only:
/*= (Mat_<int>(6,3) <<	60,61,65,	//inside of the mouth
61,64,65,
61,62,63,
61,63,64,
62,63,54,
60,65,48);*/
bool warning = false; //have we warned them about a non-66-point tracker yet?

string filename = "../images/shape";
string imgfiletype = ".jpg";
string shapefiletype = ".yml";
string imgfile, shapefile, shapefilecentre, shapefileleft, shapefileright;


void sendReplace_Face(bool replace){		//sent by the main loop to toggle face replacement
	FACEREPLACE = replace;
}



void sendFaceBackgroundBool(bool under){		//sent by the main loop to toggle background image
	UNDERLAYER = under;
}




void sendERIstrength(double texmag){	//this is called from the main simpleclm program

	ERIstrength = texmag/100.0;		//the slider is in %
};



//OpenCV -> OpenGL
// Function turn a cv::Mat into a texture, and return the texture ID as a GLuint for use
GLuint matToTexture(cv::Mat &mat)
{

	if((mat.rows == wholeimage.rows) && (mat.cols == wholeimage.cols)){
		//imshow("whole image", mat);
	}
	else if((mat.rows != FaceResolution) || (mat.cols != FaceResolution)){
		resize(mat,mat,Size(FaceResolution,FaceResolution),0,0,1);
	}


	//perhaps re-implement this so it goes a bit faster using GL pixel buffers, see http://www.songho.ca/opengl/gl_pbo.html 

	// Generate a number for our textureID's unique handle
	GLuint textureID;
	glGenTextures(1, &textureID);
	// Bind to our texture handle
	glBindTexture(GL_TEXTURE_2D, textureID);

	// glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);



	//use fast 4-byte alignment (default anyway) if possible
	glPixelStorei(GL_UNPACK_ALIGNMENT, (mat.step & 3) ? 1 : 4);

	//set length of one complete row in data (doesn't need to equal image.cols)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, mat.step/mat.elemSize());




	IplImage ipl_img = mat;

	char * buffer = new char[ipl_img.width*ipl_img.height*ipl_img.nChannels];
	int step     = ipl_img.widthStep;
	int height   = ipl_img.height;
	int width    = ipl_img.width;
	int channels = ipl_img.nChannels;
	char * data  = (char *)ipl_img.imageData;





	for(int i=0;i<height;i++)
	{
		memcpy(&buffer[i*width*channels],&(data[i*step]),width*channels);
	}

	if(channels == 3){
		glTexImage2D(GL_TEXTURE_2D, 
			0, 
			GL_RGB,
			ipl_img.width, 
			ipl_img.height, 
			0,
			GL_RGB, 
			GL_UNSIGNED_BYTE,
			buffer);
	}
	else if (channels == 4){
		glTexImage2D(GL_TEXTURE_2D, 
			0, 
			GL_RGBA,
			ipl_img.width, 
			ipl_img.height, 
			0,
			GL_RGBA, 
			GL_UNSIGNED_BYTE, 
			buffer);
	}


	glGenerateMipmapEXT(GL_TEXTURE_2D);


	delete[] buffer;

	return textureID;

}







//OpenGL-> OpenCV
//changes a screen buffer to an OpenCV matrix (so it can be post-processed or saved as a video file)
void buffertoMat(Mat &flipped)
{
	Mat img(windowHeight, windowWidth, CV_8UC3);




	//use fast 4-byte alignment (default anyway) if possible
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);

	//set length of one complete row in destination data (doesn't need to equal img.cols)
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());

	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	if (img.empty()){
		cout << "the image seems to be empty! " << endl;
	}




	flip(img, flipped, 0);


}







void cropAvatar(cv::Mat &camFrame, cv::Mat &s){				
	//This function crops down a camera frame camFrame labelled with shape points s to just the rectangle enclosing all shape points s...
	//... and resizes the cropped image to 256x256 pixels
	//It also scales/offsets the matrix of feature points s to make them refer to the new (cropped) camFrame image

	float xmin = 10000;
	float xmax = 1;
	float ymin= 10000;
	float ymax = 1;
	float valuex,valuey;
	int p = s.rows/2;

	for(int i = 0; i < p; i++){							//loop through the points and try and find the maximum and minimum 
		valuex = s.at<double>(i,0);	
		valuey = s.at<double>(i+p,0);

		if(valuex > xmax)
			xmax = valuex;
		if(valuex < xmin)
			xmin = valuex;
		if(valuey > ymax)
			ymax = valuey;
		if(valuey < ymin)
			ymin = valuey;		
	}



	if(xmin > camFrame.cols){xmin = camFrame.cols;}
	if(ymin > camFrame.rows){ymin = camFrame.rows;}
	if(xmax > camFrame.cols){xmax = camFrame.cols;}
	if(ymax > camFrame.rows){ymax = camFrame.rows;}

	xmax -= xmin;
	ymax -= ymin;

	if(xmin < 1){xmin = 1;}
	if(ymin < 1){ymin = 1;}			//this might cause some weirdness... but that's better than a divide-by-zero error.

	Rect cropped(xmin, ymin, xmax, ymax);
	Mat crop;
	Mat(camFrame,cropped).copyTo(camFrame);

	resize(camFrame, camFrame, Size(FaceResolution,FaceResolution), 0, 0, INTER_NEAREST );


	for(int i = 0; i < p; i++){
		s.at<double>(i,0) -= xmin;
		//if(s.at<double>(i,0) < 0){s.at<double>(i,0) = 0;}
		s.at<double>(i,0) *= (FaceResolution/(xmax));
		s.at<double>(i+p,0) -= ymin;
		//if(s.at<double>(i+p,0) < 0){s.at<double>(i+p,0) = 0;}
		s.at<double>(i+p,0) *= (FaceResolution/(ymax));
	}
}

void sendOptions(bool writeto, bool usesaved, string avatar){
	//This function is called by the main loop, SimplePuppets, from the GUI parameters (buttons, switches etc). They are:
	//-Toggle ERI (USESAVEDIMAGE)
	//-Write the next frame to disk as the avatar's face (WRITETODISK)
	//-Use the avatar file appended by the string called avatar, eg, for avatar=_nap the file read is shape_central_nap.yml
	if((usesaved != USESAVEDIMAGE) || (avatar != whichavatar)){
		CHANGEIMAGE = true;
		cout << "changed! " << endl;
	}

	whichavatar = avatar;
	WRITETODISK = writeto;
	USESAVEDIMAGE = usesaved;
}





void displayHistogram(string windowname, vector<cv::Mat> bgr_planes){
	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 } ;
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
	calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
	calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/histSize );

	Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

	/// Draw for each channel
	for( int i = 1; i < histSize; i++ )
	{
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
			Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
			Scalar( 255, 0, 0), 2, 8, 0  );
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
			Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
			Scalar( 0, 255, 0), 2, 8, 0  );
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
			Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
			Scalar( 0, 0, 255), 2, 8, 0  );
	}

	/// Display
	imshow(windowname, histImage );
}




//this is an alternative function for compensating colours, instead of compensateColours. It creates a lookup table to completely align the RGB histograms. 
//it is a little slower, and normally doesn't give results as good as compensateColours. However, in drastic lighting conditions it can give far superior results
void compensateColoursHistograms(Mat &compensator, Mat &compensated){
	if((compensator.channels() == 3) && (compensated.channels() == 3)){


		if(compensated.size() != blurredmask.size()){
			resize(compensated,compensated,blurredmask.size(),0,0,1);
		}



		vector<cv::Mat> channels, sourcechannels;
		split(compensated, channels);
		split(compensator, sourcechannels);



		Mat histMask = alphamask;


		/// Establish the number of bins
		int histSize = 256;

		/// Set the ranges ( for B,G,R) )
		float range[] = { 0, 256 } ;
		const float* histRange = { range };

		bool uniform = true; bool accumulate = false;

		Mat b_hist, g_hist, r_hist;




		Mat b_hist_before, g_hist_before, r_hist_before;
		calcHist( &channels[0], 1, 0, histMask, b_hist_before, 1, &histSize, &histRange, uniform, accumulate );
		calcHist( &channels[1], 1, 0, histMask, g_hist_before, 1, &histSize, &histRange, uniform, accumulate );
		calcHist( &channels[2], 1, 0, histMask, r_hist_before, 1, &histSize, &histRange, uniform, accumulate );



		//make cumulative
		for( int i = 1; i < histSize; i++ )
		{
			b_hist_before.at<float>(i) += b_hist_before.at<float>(i-1) ;
			g_hist_before.at<float>(i) += g_hist_before.at<float>(i-1) ;
			r_hist_before.at<float>(i) += r_hist_before.at<float>(i-1) ;
		}


		normalize(b_hist_before, b_hist_before, 0, 256, NORM_MINMAX, -1, Mat() );
		normalize(g_hist_before, g_hist_before, 0, 256, NORM_MINMAX, -1, Mat() );
		normalize(r_hist_before, r_hist_before, 0, 256, NORM_MINMAX, -1, Mat() );



		r_hist_before.convertTo(r_hist_before,CV_8U);


		equalizeHist(channels[0],channels[0]);
		equalizeHist(channels[1],channels[1]);
		equalizeHist(channels[2],channels[2]);

		/// Compute the histograms:
		calcHist( &sourcechannels[0], 1, 0, histMask, b_hist, 1, &histSize, &histRange, uniform, accumulate );
		calcHist( &sourcechannels[1], 1, 0, histMask, g_hist, 1, &histSize, &histRange, uniform, accumulate );
		calcHist( &sourcechannels[2], 1, 0, histMask, r_hist, 1, &histSize, &histRange, uniform, accumulate );






		//make cumulative
		for( int i = 1; i < histSize; i++ )
		{
			b_hist.at<float>(i) += b_hist.at<float>(i-1) ;
			g_hist.at<float>(i) += g_hist.at<float>(i-1) ;
			r_hist.at<float>(i) += r_hist.at<float>(i-1) ;
		}




		normalize(b_hist, b_hist, 0, 256, NORM_MINMAX, -1, Mat() );
		normalize(g_hist, g_hist, 0, 256, NORM_MINMAX, -1, Mat() );
		normalize(r_hist, r_hist, 0, 256, NORM_MINMAX, -1, Mat() );

		Mat bHistNew,gHistNew,rHistNew;

		b_hist.convertTo(bHistNew,CV_8U);
		g_hist.convertTo(gHistNew,CV_8U);
		r_hist.convertTo(rHistNew,CV_8U);



		Mat Bout,Rout,Gout;

		Mat bHistInv(1,256,CV_8U);
		Mat gHistInv(1,256,CV_8U);
		Mat rHistInv(1,256,CV_8U);

		//cout << "*" << bHistNew << endl;

		for(int i = 1; i < 256; i++){
			int firstindex = bHistNew.at<uchar>(i-1);
			int secondindex = bHistNew.at<uchar>(i);
			//cout << "i: " << i << ", first index: " << firstindex << ", second index: " << secondindex << endl;
			for(int f = firstindex; f <= secondindex; f++){
				bHistInv.at<uchar>(f) = i;
				//cout << "     f: " << f << ", i: " << i << endl;
			}
		}



		for(int i = 1; i < 256; i++){
			int firstindex = gHistNew.at<uchar>(i-1);
			int secondindex = gHistNew.at<uchar>(i);
			for(int f = firstindex; f <= secondindex; f++){
				gHistInv.at<uchar>(f) = i;
			}
		}	



		for(int i = 1; i < 256; i++){
			int firstindex = rHistNew.at<uchar>(i-1);
			int secondindex = rHistNew.at<uchar>(i);
			for(int f = firstindex; f <= secondindex; f++){
				rHistInv.at<uchar>(f) = i;
			}
		}

		//cout << "*" << bHistInv << endl;



		Mat histImage( 256, 256, CV_8UC3, Scalar( 0,0,0) );





		LUT(channels[0],bHistInv,Bout);
		LUT(channels[1],gHistInv,Gout);
		LUT(channels[2],rHistInv,Rout);

		if(!Bout.empty()){
			Bout.copyTo(channels[0]);
			Gout.copyTo(channels[1]);
			Rout.copyTo(channels[2]);
		}




		Mat b_hist_after, g_hist_after, r_hist_after;
		calcHist( &channels[0], 1, 0, histMask, b_hist_after, 1, &histSize, &histRange, uniform, accumulate );
		calcHist( &channels[1], 1, 0, histMask, g_hist_after, 1, &histSize, &histRange, uniform, accumulate );
		calcHist( &channels[2], 1, 0, histMask, r_hist_after, 1, &histSize, &histRange, uniform, accumulate );


		//make cumulative
		for( int i = 1; i < histSize; i++ )
		{
			b_hist_after.at<float>(i) += b_hist_after.at<float>(i-1) ;
			g_hist_after.at<float>(i) += g_hist_after.at<float>(i-1) ;
			r_hist_after.at<float>(i) += r_hist_after.at<float>(i-1) ;
		}

		normalize(b_hist_after, b_hist_after, 0, 256, NORM_MINMAX, -1, Mat() );
		normalize(g_hist_after, g_hist_after, 0, 256, NORM_MINMAX, -1, Mat() );
		normalize(r_hist_after, r_hist_after, 0, 256, NORM_MINMAX, -1, Mat() );


		r_hist_after.convertTo(r_hist_after,CV_8U);

		/// Draw for each channel
		for( int i = 1; i < histSize; i++ )
		{





			line( histImage, Point( i-1 , r_hist_before.at<uchar>(i-1) ) ,
				Point(i , r_hist_before.at<uchar>(i)),
				Scalar( 255, 0, 255), 2, 8, 0  );



			line( histImage, Point( i-1 , rHistNew.at<uchar>(i-1) ) ,
				Point(i , rHistNew.at<uchar>(i)),
				Scalar( 255, 0, 0), 2, 8, 0  );



			line( histImage, Point( i-1 , rHistInv.at<uchar>(i-1) ) ,
				Point(i , rHistInv.at<uchar>(i)),
				Scalar( 255, 255, 0), 2, 8, 0  );

			line( histImage, Point( i-1 , r_hist_after.at<uchar>(i-1) ) ,
				Point(i , r_hist_after.at<uchar>(i)),
				Scalar( 0, 255, 255), 2, 8, 0  );
		}

		imshow("histImage",histImage);









		merge(channels, compensated);



		split(compensated, channels);

		if(UNDERLAYER){
			channels.push_back(blurredmask / 1.3f);
		}
		else{
			channels.push_back(blurredmask);
		}

		merge(channels, compensated);




	}


}


void addAlphaMask(Mat &opaque){
	if(opaque.channels() == 3){
		if(opaque.size() != blurredmask.size()){
			resize(opaque,opaque,blurredmask.size(),0,0,1);
		}
		vector<Mat> channels;
		split(opaque,channels);
		if(UNDERLAYER){
			channels.push_back(blurredmask / 1.3f);
		}
		else{
			channels.push_back(blurredmask);
		}

		merge(channels,opaque);

	}
}




void compensateColours(Mat &compensator, Mat &compensated){
	if((compensator.channels() == 3) && (compensated.channels() == 3)){






		if(compensated.size() != blurredmask.size()){
			resize(compensated,compensated,blurredmask.size(),0,0,1);
		}






		Mat histMask = alphamask;


		Mat meanComp,stddevComp;
		Mat meanRef,stddevRef;



		meanStdDev(compensator, meanRef, stddevRef, Mat());

		vector<cv::Mat> channels, refchannels;

		split(compensated, channels);

		if(DISPLAYHISTOGRAMS){
			split(compensator, refchannels);
			displayHistogram("source",channels);
			displayHistogram("ref",refchannels);
		}


		for(int c = 0; c < 3; c++){

			channels[c].convertTo(channels[c],CV_32F);

			Mat oldmean,oldstddev;

			meanStdDev(channels[c], oldmean, oldstddev, Mat());
			float scale = stddevRef.at<double>(c,0) / oldstddev.at<double>(0,0);


			channels[c] *= scale;

			float offset = meanRef.at<double>(c,0) - (scale*oldmean.at<double>(0,0));

			channels[c] += offset;


			channels[c].convertTo(channels[c],CV_8U);

		}

		if(DISPLAYHISTOGRAMS){
			displayHistogram("compensated",channels);
		}

		if(UNDERLAYER){
			channels.push_back(blurredmask / 1.3f);
		}
		else{
			channels.push_back(blurredmask);
		}

		merge(channels, compensated);




	}


}

void draw(cv::Mat camFrame, cv::Mat depthFrame, cv::Mat &s,cv::Mat &savatar,cv::Mat &snew, cv::Mat &_tri, Mat avatarlocal, bool ERIon, int posture)
{

	bool CHANGEPOSTURE = false;


	//	wholeimage.resize(512,512);
	
	if(WRITETODISK){		//WRITETODISK is a condition given by the GUI when an appropriate button is pressed

		cout << "writing to disk..." << endl;
		Mat scropped;
		s.copyTo(scropped);
		cout << "posture: " <<posture << endl;
		cout << "framecout: " <<frameCount <<  endl;

		if(posture==0){
			cout << "got in here! " << endl;						//centre file. This file should have the links to the other YLMs
			imgfile = filename +"_central"+ imgfiletype;
			shapefile = filename +"_central"+ shapefiletype;
			shapefileleft = filename +"_central"+ shapefiletype;		//change "_central" to _left or _right to go back to multi-view avatars!!!
			shapefileright = filename +"_central"+ shapefiletype;
			FileStorage fs(shapefile, FileStorage::WRITE);
			imwrite(imgfile, depthFrame );
			fs << "shape" << scropped;
			fs << "filename" << imgfile;
			fs << "fileleft" << shapefileleft;
			fs << "fileright" << shapefileright;
			fs.release();
		}


		if(posture==2){
			imgfile = filename+ "_left" + imgfiletype;
			shapefile = filename+ "_left" + shapefiletype;
			FileStorage fs(shapefile, FileStorage::WRITE);
			imwrite(imgfile, depthFrame );
			fs << "shape" << scropped;
			fs << "filename" << imgfile;
			fs.release();
		}


		if(posture==1){
			imgfile = filename +"_right"+ imgfiletype;
			shapefile = filename +"_right"+ shapefiletype;
			FileStorage fs(shapefile, FileStorage::WRITE);
			imwrite(imgfile, depthFrame );
			fs << "shape" << scropped;
			fs << "filename" << imgfile;
			fs.release();
		}

		//the 'smiling' and 'openmouth' textures are no longer used

		cout << "saved to: " << shapefile << endl;
		WRITETODISK = false;
		CHANGEIMAGE = true;


		glFinish();
		glDisable(GL_TEXTURE_2D);
	}




	if(!FACEREPLACE && (posture == 0)){
		compensateColours(camFrame,depthFrame);
	}



	if(mouthtri.empty() || eyestri.empty()){
		string mouthfile = "./model/mouthEyesShape.yml";
		cout << "Reading mouth and eyes triangles from " << mouthfile << endl;
		FileStorage fsc(mouthfile, FileStorage::READ);		
		fsc["eyestri"] >> eyestri;
		fsc["mouthtri"] >> mouthtri;
		fsc.release();
	}




	if(UNDERLAYER){

		Mat blurredAvatar2;
		Mat depthFrameCopy;
		resize(depthFrame,depthFrameCopy,Size(64,64),0,0,1);
		GaussianBlur(depthFrameCopy,blurredAvatar2,Size(0,0),7,7,4);
		resize(blurredAvatar2,blurredAvatar2,Size(128,128),0,0,1);

		addAlphaMask(blurredAvatar2);

		glDeleteTextures(1, &texblurred);
		texblurred = matToTexture(blurredAvatar2);



	}



	if(oldposture != posture)
	{
		CHANGEPOSTURE = true;		//need to read the new posture's image!
		cout << "posture changed. New posture: " << posture << endl;
	}


	oldposture = posture;
	//if none, posture==0 indicates central.






	if(USESAVEDIMAGE && CHANGEIMAGE){
		cout << "Using loaded avatar." << endl;



		glDeleteTextures( 1, &texcentre );
		glDeleteTextures( 1, &texleft );
		glDeleteTextures( 1, &texright );


		string readloc, readlocleft, readlocright;

		shapefile = filename+"_central" + whichavatar+ shapefiletype;
		FileStorage fsc(shapefile, FileStorage::READ);		
		fsc["shape"] >> scopycentre;
		fsc["filename"] >> readloc;
		fsc["fileleft"] >> readlocleft;
		fsc["fileright"] >> readlocright;
		fsc.release();
		centreAvatar = imread( readloc );

		if(FACEREPLACE){
			compensateColours(depthFrame,centreAvatar);
		}

		//cropAvatar(centreAvatar,scopycentre);
		texcentre = matToTexture(centreAvatar);


		FileStorage fsl(readlocleft, FileStorage::READ);	
		fsl["shape"] >> scopyleft;
		fsl["filename"] >> readloc;
		fsl.release();
		leftAvatar = imread( readloc );

		if(FACEREPLACE){
			compensateColours(depthFrame,leftAvatar);
		}
		texleft = matToTexture(leftAvatar);




		FileStorage fsr(readlocright, FileStorage::READ);	
		fsr["shape"] >> scopyright;
		fsr["filename"] >> readloc;
		fsr.release();
		rightAvatar = imread( readloc );
		//cropAvatar(rightAvatar,scopyright);
		if(FACEREPLACE){
			compensateColours(depthFrame,rightAvatar);
		}
		texright = matToTexture(rightAvatar);


		CHANGEPOSTURE = true;

		CHANGEIMAGE = false;


	}


	if(!FACEREPLACE){
		if(posture == 0){
			compensateColours(centreAvatar,depthFrame);
		}
		if(posture == 2){
			compensateColours(leftAvatar,depthFrame);
		}
		if(posture == 1){
			compensateColours(rightAvatar,depthFrame);
		}
	}





	if(USESAVEDIMAGE && CHANGEPOSTURE){
		cout << "CHANGING POSTURE AND USING SAVED IMAGE" << endl;
		if(posture == 0){
			imageTex = texcentre;
			scopycentre.copyTo(scopy);
			widthavat = centreAvatar.cols;
			heightavat = centreAvatar.rows;
		}

		if(posture == 2){
			imageTex = texleft;
			scopyleft.copyTo(scopy);
			widthavat = leftAvatar.cols;
			heightavat = leftAvatar.rows;
		}	

		if(posture == 1){
			imageTex = texright;
			scopyright.copyTo(scopy);
			widthavat = rightAvatar.cols;
			heightavat = rightAvatar.rows;
		}



	}




	glDeleteTextures(1, &texbackground);



	texbackground = matToTexture(wholeimage);

	compensateColours(depthFrame,camFrame);



	// Clear the screen and depth buffer, and reset the ModelView matrix to identity
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	// Move things onto the screen
	glTranslatef(-1.0f, +1.0f, -0.0f);

	glScalef(2.0f, -2.0f ,1.0f);							//this is because opengl's pixel order is upside-down

	glEnable(GL_TEXTURE_2D);

	// Every (framesTick) frames, convert image data to OpenGL textures to fill in the mouth. This currently happens every frame
	if(frameCount % framesTick == 0){
		depthFrame.copyTo(camFrameCopy);
		s.copyTo(seyes);


		if(!FACEREPLACE){
			glDeleteTextures( 1, &texmouth );
			texmouth = matToTexture(camFrameCopy);
		}

	}



	float eyeswidth = wholeimage.cols;
	float eyesheight = wholeimage.rows;



	//If using saved image and every framesTick frames, or alternatively if the posture changes (you suddenly look to the left)
	if(((!USESAVEDIMAGE) && (frameCount % framesTick == 0)) || (!USESAVEDIMAGE && CHANGEPOSTURE)){


		if(ERIon){
			snew.copyTo(scopy);
			glDeleteTextures( 1, &imageTex );
			imageTex = matToTexture(camFrame);
			widthavat = camFrame.cols;
			heightavat = camFrame.rows;
		}
		else{	
			Mat croppedavatar;
			s.copyTo(scopy);
			glDeleteTextures( 1, &imageTex );
			imageTex = matToTexture(depthFrame);
			widthavat = depthFrame.cols;
			heightavat = depthFrame.rows;
		}


	}


	int p = (s.rows/2);

	if(FACEREPLACE){	
		glEnable (GL_BLEND);
	}
	else{
		glDisable(GL_BLEND);
	}



	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	if(FACEREPLACE){

		glBindTexture(GL_TEXTURE_2D, texbackground);	//change to texmouth to fill-in the mouth (eyes are filled in from the avatar, mouth from the source video/camera)
		glBegin(GL_TRIANGLES);


		float xmax = 1.0;
		float ymax = 1.0;

		glTexCoord2f(0.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);

		glTexCoord2f(1.0f,0.0f);
		glVertex3f(xmax,0.0f,0.0f);

		glTexCoord2f(0.0f,1.0f);
		glVertex3f(0.0f,ymax,0.0f);

		glTexCoord2f(0.0f,1.0f);
		glVertex3f(0.0f,ymax,0.0f);

		glTexCoord2f(1.0f,0.0f);
		glVertex3f(xmax,0.0f,0.0f);

		glTexCoord2f(1.0f,1.0f);
		glVertex3f(xmax,ymax,0.0f);


		glEnd();

	}




	if(!FACEREPLACE){

		glBindTexture(GL_TEXTURE_2D, texmouth);	//change to texmouth to fill-in the mouth (eyes are filled in from the avatar, mouth from the source video/camera)
		glBegin(GL_TRIANGLES);

		for(int l=0;l< mouthtri.rows;l++){ 

			int	i = mouthtri.it(l,0);
			int	j = mouthtri.it(l,1);
			int	k = mouthtri.it(l,2);

			glTexCoord2f((seyes.db(i,0))/FaceResolution, (seyes.db(i+p,0))/FaceResolution);
			glVertex3f(savatar.db(i,0)/width, savatar.db(i+p,0)/height,0.0f);

			glTexCoord2f((seyes.db(j,0))/FaceResolution, (seyes.db(j+p,0))/FaceResolution);
			glVertex3f(savatar.db(j,0)/width, savatar.db(j+p,0)/height,0.0f);

			glTexCoord2f((seyes.db(k,0))/FaceResolution, (seyes.db(k+p,0))/FaceResolution);
			glVertex3f(savatar.db(k,0)/width, savatar.db(k+p,0)/height,0.0f);


		}



		glEnd(); 
	}



	//enable cull facing (don't bother rendering the back of triangles) for the main face - they're all well-behaved. Disable it later for mouth filling
	
	if(s.rows == 132){
		glEnable(GL_CULL_FACE);
	}
	else if(!warning){
		cout << "ERROR: 66 POINT MODE NOT DETECTED. Disableing face culling. Make sure you have loaded the right mouth-matrix-model into lib/local/Puppets/model/mouthEyesShape.yml" << endl;
		warning = true;
	}



	if(UNDERLAYER){
		//bind the imageTex (which is whichever avatar we're using at the moment)
		glBindTexture(GL_TEXTURE_2D, texblurred);


		glBegin(GL_TRIANGLES);


		for(int l=0;l< _tri.rows;l++){	//cycle along the appropriate face triangles (and x-y points from s) and map them using glTexCoord-glVertex 

			int	i = _tri.it(l,0);
			int	k = _tri.it(l,1);
			int	j = _tri.it(l,2);

			glTexCoord2f((scopy.db(i,0))/FaceResolution, (scopy.db(i+p,0))/FaceResolution);
			glVertex3f(savatar.db(i,0)/width, savatar.db(i+p,0)/height,0.0f);

			glTexCoord2f((scopy.db(j,0))/FaceResolution, (scopy.db(j+p,0))/FaceResolution);
			glVertex3f(savatar.db(j,0)/width, savatar.db(j+p,0)/height,0.0f);

			glTexCoord2f((scopy.db(k,0))/FaceResolution, (scopy.db(k+p,0))/FaceResolution);
			glVertex3f(savatar.db(k,0)/width, savatar.db(k+p,0)/height,0.0f);

		}



		glEnd();

	}



	glBindTexture(GL_TEXTURE_2D, imageTex);



	glBegin(GL_TRIANGLES);


	for(int l=0;l<_tri.rows; l++){	//cycle along the appropriate face triangles (and x-y points from s) and map them using glTexCoord-glVertex 

		int	i = _tri.it(l,0);
		int	k = _tri.it(l,1);
		int	j = _tri.it(l,2);

		glTexCoord2f((scopy.db(i,0))/FaceResolution, (scopy.db(i+p,0))/FaceResolution);
		glVertex3f(savatar.db(i,0)/width, savatar.db(i+p,0)/height,0.0f);

		glTexCoord2f((scopy.db(j,0))/FaceResolution, (scopy.db(j+p,0))/FaceResolution);
		glVertex3f(savatar.db(j,0)/width, savatar.db(j+p,0)/height,0.0f);

		glTexCoord2f((scopy.db(k,0))/FaceResolution, (scopy.db(k+p,0))/FaceResolution);
		glVertex3f(savatar.db(k,0)/width, savatar.db(k+p,0)/height,0.0f);

	}



	glEnd();


	//	glDisable (GL_BLEND);



	glDisable(GL_CULL_FACE);		//for the next part (the eyes and the mouth fill-in, we're not sure which way around the triangles go

	glBegin(GL_TRIANGLES);

	for(int l=0;l< eyestri.rows;l++){

		int	i = eyestri.it(l,0);		//read from the eyestri matrix - copy the eyes from the avatar image (still active texture)
		int	j = eyestri.it(l,1);
		int	k = eyestri.it(l,2);


		glTexCoord2f((scopy.db(i,0))/widthavat, (scopy.db(i+p,0))/heightavat);
		glVertex3f(savatar.db(i,0)/width, savatar.db(i+p,0)/height,0.0f);

		glTexCoord2f((scopy.db(j,0))/widthavat, (scopy.db(j+p,0))/heightavat);
		glVertex3f(savatar.db(j,0)/width, savatar.db(j+p,0)/height,0.0f);

		glTexCoord2f((scopy.db(k,0))/widthavat, (scopy.db(k+p,0))/heightavat);
		glVertex3f(savatar.db(k,0)/width, savatar.db(k+p,0)/height,0.0f);


	}

	glEnd();





	glDisable(GL_TEXTURE_2D);




	glFinish();







}




void initGL(int argc, char* argv[])
{

	

	glewExperimental = TRUE;



	// Initialise glfw
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE);
	glutInitWindowSize(windowWidth,windowHeight); 
	glutCreateWindow("Puppets");



	glewInit();


	alphamask = Mat::zeros(256,256, CV_8UC1);
	circle(alphamask, Point(64,64), 40, Scalar(200), -1, 8, 0); 
	circle(alphamask, Point(192,64), 40, Scalar(200), -1, 8, 0); 
	circle(alphamask, Point(128,128), 110, Scalar(255), -1, 8, 0); 
	GaussianBlur(alphamask,blurredmask,Size(0,0),11,11,4);

	resize(alphamask, alphamask, Size(128,128),0,0,1);
	resize(blurredmask, blurredmask, Size(128,128),0,0,1);


	// Setup our viewport to be the entire size of the window
	glViewport(0, 0, windowWidth,windowHeight);

	// Change to the projection matrix and set our viewing volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspectRatio = (windowWidth > windowHeight)? float(windowWidth)/float(windowHeight) : float(windowHeight)/float(windowWidth);
	GLfloat fH = tan( float(fieldOfView / 360.0f * 3.14159f) ) * zNear;
	GLfloat fW = fH * aspectRatio;
//	glFrustum(-fW, fW, -fH, fH, zNear, zFar);

	// ----- OpenGL settings -----

	glDepthFunc(GL_LEQUAL);		// Specify depth function to use

	glEnable(GL_DEPTH_TEST);    // Enable the depth buffer

//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Ask for nicest perspective correction

	//	glEnable(GL_CULL_FACE);     // Cull back facing polygons


	// Switch to ModelView matrix and reset
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Set our clear colour to black
	if(KIOSKMODE){
		 glutHideWindow();
	}
}



void resetERIExpression(){
	resetexpression = true;
}


Mat DoOpenglStuff(Mat dst,Mat s,Mat savatar,Mat snew, Mat _tri,  Mat avatarlocal, Mat warpedFace, Mat warpedAvatar, int posture, bool useglue){

	KIOSKMODE = !useglue;

	dst.copyTo(wholeimage);

	windowWidth = wholeimage.cols;
	windowHeight = wholeimage.rows;





	width = windowWidth;
	height = windowHeight;




	Mat sothercropped;
	s.copyTo(sothercropped);
	cropAvatar(dst, sothercropped);

	if(dst.size() !=Size(128,128)){
		resize(dst,dst,Size(128,128),0,0,INTER_NEAREST);
	}
	if(warpedFace.size() !=Size(128,128)){
		resize(warpedFace,warpedFace,Size(128,128),0,0,INTER_NEAREST);
	}
	if(warpedAvatar.size() !=Size(128,128)){
		resize(warpedAvatar,warpedAvatar,Size(128,128),0,0,INTER_NEAREST);
	}


	Mat ycrcb;

	cvtColor(warpedFace,ycrcb,CV_BGR2YCrCb);
	vector<Mat> channels;
	split(ycrcb,channels);

	equalizeHist(channels[0], channels[0]);

	merge(channels,ycrcb);

	cvtColor(ycrcb,warpedFace,CV_YCrCb2BGR);


	if(INIT != true ) {

		if(RECORDVIDEO){
			cout << "Recording Video. Width: " << windowWidth << ", Height: " << windowHeight << endl;
			outputWidth = windowWidth; 
			outputHeight = windowHeight;
			myvideo.open("..\\videos\\outputvideo.avi", 0, 15, Size(outputWidth,outputHeight), true);
			cout << "Video recording initialisation successful? : " << myvideo.isOpened() << endl;
		}
		char *myargv [1];
		int myargc=1;
		myargv [0]=strdup ("Myappname");
		initGL(1, myargv);
		INIT = true;
	}


	else if(INIT == true ) {


		if((frameCount == 10) || resetexpression){	
			resetexpression = false;
			GaussianBlur( warpedFace, warpedFace, Size( 15, 15 ), 0, 0 );							//get standard expression

			if(posture == 0){
				warpedFace.convertTo(oldFace, CV_32FC3);
				//	imshow("mid face", oldFace);
			}
			if(posture == 2){
				warpedFace.convertTo(oldFaceLeft, CV_32FC3);
				//	imshow("mid face left", oldFaceLeft);
			}
			if(posture == 1){
				warpedFace.convertTo(oldFaceRight, CV_32FC3);
				//	imshow("mid face right", oldFaceRight);
			}


		}




		if(((frameCount > 10 ) && !USESAVEDIMAGE && ((frameCount % framesTick) == 0)) || frameCount == 11 ){			//get current expression



			resize(warpedFace,warpedFace,Size(64,64),0,0,INTER_NEAREST  );							//for efficient blurring, let's downsize the image (nearest-neighbour is fine)
			GaussianBlur( warpedFace, newWarpedFace, Size( 7,7 ), 0, 0 );								//now we blur... (kernel can be small too)
			resize(newWarpedFace,newWarpedFace,Size(FaceResolution,FaceResolution),0,0,INTER_LINEAR  );				//and now we scale it back up. better use linear interp. to avoid weird stuff

			newWarpedFace.convertTo(newWarpedFaceFloat, CV_32FC3);

			Mat newWarpedFaceFloat_yuv, oldFace_yuv;
			// convert image to YUV color space.
			// The output image will be allocated automatically
			//cvtColor(newWarpedFaceFloat,newWarpedFaceFloat,CV_RGB2BGR);
			cvtColor(newWarpedFaceFloat, newWarpedFaceFloat_yuv, CV_RGB2YCrCb);
			cvtColor(oldFace, oldFace_yuv, CV_RGB2YCrCb);

			// split the image into separate color planes
			vector<Mat> newWarpedFaceFloat_planes, oldFace_planes;
			split(newWarpedFaceFloat_yuv, newWarpedFaceFloat_planes);
			split(oldFace_yuv, oldFace_planes);


			//if(posture == 0){		
			divide(newWarpedFaceFloat_planes[0], oldFace_planes[0], ratioFaceFloat);	//always do this... just in case!
			//}
			/*
			if((posture == -1) && !oldFaceLeft.empty()){
			divide(newWarpedFaceFloat, oldFaceLeft, ratioFaceFloat);
			}
			if((posture == 1) && !oldFaceRight.empty()){
			divide(newWarpedFaceFloat, oldFaceRight, ratioFaceFloat);
			}*/


			Mat lessthan = (ratioFaceFloat <= 1.0f);
			lessthan.convertTo(lessthan,CV_8U);
			Mat ratioFaceFloatMask;		
			ratioFaceFloat -= Scalar(1.0f);
			ratioFaceFloat.copyTo(ratioFaceFloatMask,lessthan);		//applies the 'less than' mask!
			ratioFaceFloatMask *= ERIstrength;
			ratioFaceFloatMask += Scalar(1.0f);
			ratioFace = ratioFaceFloatMask * 100.0;
			ratioFace.convertTo(ratioFace, CV_8UC3);
			//cvtColor(ratioFace,ratioFace,CV_RGB2BGR);
			//GaussianBlur( ratioFaceFloatMask, ratioFaceFloatMask, Size( 15, 15 ), 0, 0 );	

			//warpedAvatar.convertTo(avatarFace, CV_32FC3);
			resize(warpedAvatar, newWarpedFace, oldFace.size(), 0, 0, INTER_NEAREST );


			Mat avatarFace_yuv;
			cvtColor(newWarpedFace, avatarFace_yuv, CV_RGB2YCrCb);
			vector<Mat> avatarFace_channels;
			split(avatarFace_yuv, avatarFace_channels);

			avatarFace_channels[0].convertTo(avatarFace_channels[0], CV_32F);

			multiply(avatarFace_channels[0], ratioFaceFloatMask, newFace);

			avatarFace_channels[0] = newFace;

			avatarFace_channels[0].convertTo(avatarFace_channels[0], CV_8U);

			// now merge the results back
			merge(avatarFace_channels, newFace);
			// and produce the output RGB image
			cvtColor(newFace, newFace, CV_YCrCb2BGR);



			//Mat rgbnewface;
			//cvtColor(newFace,rgbnewface,CV_BGR2RGB);
			//imshow("new face",rgbnewface);

		}




		if(!avatarlocal.empty()){


			int p=snew.rows/2;

			for(int i = 0; i < p; i++){
				snew.db(i,0) += 22;
				snew.db(i,0) *= (FaceResolution/44.0);					//reposition the shape-matrix so that the face is nicely in the middle when we send it to OpenGL
				snew.db(i+p,0) += 20;
				snew.db(i+p,0) *= (FaceResolution/45.0);
			}

			if(dst.size() != Size(FaceResolution,FaceResolution)){
				resize(dst,dst,Size(FaceResolution,FaceResolution),0,0,INTER_NEAREST  );
			}



			if((frameCount < 11) || (posture != 0)){
				draw(dst,dst,sothercropped,savatar, snew, _tri, avatarlocal, 0, posture);
			}

			else{



				if(newFace.size() != Size(FaceResolution,FaceResolution)){
					resize(newFace,newFace,Size(FaceResolution,FaceResolution),0,0,INTER_NEAREST  );
				}






				draw(newFace,dst,sothercropped,savatar, snew, _tri, avatarlocal, 1, posture);

			}

		}


		frameCount++;

		glutSwapBuffers();


					Mat picture;

			buffertoMat(picture);


		if(RECORDVIDEO){

			if ( !myvideo.isOpened() ) //if not initialize the VideoWriter successfully, exit the program
			{
				cout << "ERROR: Failed to write the video" << endl;
			}




			if(!picture.empty()){
				resize(picture,picture,Size(outputWidth,outputHeight),0,0,1);
				myvideo.write(picture);
			}


		}


		return picture;


	}

	return Mat();
}