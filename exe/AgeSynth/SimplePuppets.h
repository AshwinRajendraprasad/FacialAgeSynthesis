
#ifndef __AGESYNTH_h_
#define __AGESYNTH_h_



#include <CLMTracker.h>
#include <PoseDetectorHaar.h>
#include <Avatar.h>
#include <PAW.h>
#include <CLM.h>

#include <gtk/gtk.h>
#include <fstream>
#include <sstream>

#include <cv.h>



	GtkWidget *window;
	GtkWidget *testwindow;
    GtkWidget *button;
    GtkWidget *table;
	GtkWidget *drawing_area;
    GtkWidget *check, *check1, *check2;
	GtkWidget *hscale, *hscale2, *hscale3, *hscale4, *hscale5, *label1, *label2, *label3, *label4, *label5, *avatarchoice, *inputchoice;
	GtkObject *adj1, *adj2, *adj3, *adj4, *adj5;

  int mindreadervideo = -1;

  void readFromStock(int c);

bool writeToFile = 0;
bool ERIon = 0;
bool quitmain = 0;
string choiceavatar = "0";
bool GRAYSCALE = false;

int option, oldoption;

GtkWidget *filew, *filez;
int facesInRow = 0;
bool trackingInitialised;

string inputfile;
bool NEWFILE = false;
VideoCapture vCap;

#define INFO_STREAM( stream ) \
std::cout << stream << std::endl

#define WARN_STREAM( stream ) \
std::cout << "Warning: " << stream << std::endl

#define ERROR_STREAM( stream ) \
std::cout << "Error: " << stream << std::endl


IplImage opencvImage;
GdkPixbuf* pix;
bool USEWEBCAM = false;
bool CHANGESOURCE = false;
bool PAWREADAGAIN = false;
bool PAWREADNEXTTIME = false;



void use_webcam();
static gboolean time_handler( GtkWidget *widget );
gboolean expose_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
static void file_ok_sel( GtkWidget        *w,  GtkFileSelection *fs );
static void file_ok_sel_z( GtkWidget        *w,  GtkFileSelection *fs );
static void callback( GtkWidget *widget,  gpointer   data );
static gboolean delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data );
static void printErrorAndAbort( const std::string & error );
Matx33f Euler2RotationMatrix(const Vec3d& eulerAngles);
void Project(Mat_<float>& dest, const Mat_<float>& mesh, Size size, double fx, double fy, double cx, double cy);
void DrawBox(Mat image, Vec6d pose, Scalar color, int thickness, float fx, float fy, float cx, float cy);
vector<string> get_arguments(int argc, char **argv);




void doFaceTracking(int argc, char **argv);
void startGTK(int argc, char **argv);
int main (int argc, char **argv);
void Puppets();




#endif
