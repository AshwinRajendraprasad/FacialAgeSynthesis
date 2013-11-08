

#ifndef __AVATAR_h_
#define __AVATAR_h_



#include <GL/glut.h>
#include <cv.h>
#include <PAW.h>


using namespace std;

#define it at<int>
#define db at<double>

using namespace cv;




void sendOptions(bool writeto, bool usedsave, string avatar);

void resetERIExpression();

void cropAvatar(cv::Mat &m, cv::Mat &s);
 
Mat DoOpenglStuff(Mat dst,Mat s,Mat savatar,Mat snew, Mat _tri, Mat localshape, Mat warpedFace, Mat warpedAvatar, int posture, bool useglue);

void draw(cv::Mat camFrame, cv::Mat depthFrame, cv::Mat &s,cv::Mat &savatar,cv::Mat &snew, cv::Mat &_tri,  Mat avatarlocal, bool ERIon, int posture);

void initGL(int argc, char* argv[]);

void sendERIstrength(double texmag);

GLuint matToTexture(cv::Mat &mat);

void compensateColours(Mat &compensator, Mat &compensated);

void compensateColoursHistograms(Mat &compensator, Mat &compensated);

void addAlphaMask(Mat &threechannel);

void buffertoMat(Mat &flipped);

void sendReplace_Face(bool replace);

void sendFaceBackgroundBool(bool under);



#endif


