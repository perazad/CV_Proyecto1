//Project:		PTZR Camara en OpenGL
//File:			project01.cpp
//Authors:		Andres Gonzalez, David Peraza, Mariano Jimenez
//Date:			07/01/2016
//Course:		PDI
//Place:		ITCR

//General libraries
#include <math.h>

//OpenGL and OpenCV libraries
#include <GL/glut.h>
#include "highgui.h"

//Control variables
static GLfloat spin = 0.0;
static GLfloat theta = 0.0;
static GLfloat movex = 0.0;
static GLfloat movey = 0.0;
static GLfloat movexd = 0.0;
static GLfloat moveyd = 0.0;
static GLfloat movexLast = 0.0;
static GLfloat moveyLast = 0.0;
static GLfloat xTextFact = 0.0007663;
static GLfloat yTextFact = 0.0009050;
static GLfloat xT1 = 0.617;
static GLfloat yT1 = 0.639;
static GLfloat xT2 = 0.383;
static GLfloat yT2 = 0.361;
static GLfloat xT = 150;
static GLfloat yT = 150;


GLuint listIndex;
GLuint texture;
CvCapture* capture;

bool rotate = false;
bool moveInX = false;
bool moveInY = false;
double width = 1280;
double height = 1080;
double zoom = 1.0;

#define PI 3.14159265

//Convert camera capture to texture
GLuint ConvertIplToTexture(IplImage *image)
{
  GLuint texture;

  glGenTextures(1,&texture);
  glBindTexture(GL_TEXTURE_2D,texture);
  gluBuild2DMipmaps(GL_TEXTURE_2D,3,image->width,image->height,
  GL_BGR,GL_UNSIGNED_BYTE,image->imageData);

 return texture;
 
}

GLvoid DrawRect()
{
	IplImage* frame;
	frame = cvQueryFrame(capture);
	texture = ConvertIplToTexture(frame);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_POLYGON);
		glTexCoord2f(xT1, yT1);
		glVertex3f(-(300 / 2), -(300 / 2), 0.0);
		glTexCoord2f(xT1, yT2);
		glVertex3f(-(300 / 2), (300 / 2), 0.0);
		glTexCoord2f(xT2, yT2);
		glVertex3f((300 / 2), (300 / 2), 0.0);
		glTexCoord2f(xT2, yT1);
		glVertex3f((300 / 2), -(300 / 2), 0.0);		
	glEnd();	
	glDisable(GL_TEXTURE_2D);
}

void Initialize_OpenCV() {//begins
	
	capture = cvCaptureFromCAM(1);	//device video1
	
}

void Init(void) {//begins

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

}//Init function  ends

void Display(void) {//begins

	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();
	//gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -100.0, 0.0, 1.0, 0.0);
	glRotatef(spin, 0.0, 0.0, 1.0);			//rotation transformation
	//glTranslatef(movexd, moveyd, 0.0);    //translation transformation
	glTranslatef(0.0, 0.0, 0.0);    //translation transformation
	glScalef(zoom, zoom, 1.0);				//scale transformation
	glColor3f(1.0, 1.0, 1.0);
	DrawRect();
	glPopMatrix();
	glutSwapBuffers();
	glFlush();

}//Display function ends

void SpinDisplay(void) {//begins
	
	spin = rotate ? spin + 2.0 : spin;
	
	if(spin > 360.0)
		spin = spin - 360.0;
		
	glutPostRedisplay();

}//SpinDisplay function ends

void Reshape(int w, int h) {//begins

	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(width / 2), (width / 2), -(height / 2), (height / 2), -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}//Reshape function ends

void Mouse(int button, int state, int x, int y) {//begins

	switch(button) {//begins
	
		case GLUT_LEFT_BUTTON:
			
			if(state == GLUT_DOWN) {
				
				rotate = true;
				glutIdleFunc(SpinDisplay);
				
			}
				
			break;
			
		case GLUT_RIGHT_BUTTON:
		
			if(state == GLUT_DOWN) {//begins
				
				rotate = false;
				glutIdleFunc(SpinDisplay);
				
			}
				
			break;
			
		default:
			
			break;
			
	}//switch(button) ends

}//Mouse function ends

void Keyboard(unsigned char key, int x, int y) {//begins

	theta = spin * PI / 180;
	
	switch(key) {//begins
	
		case 's':
		case 'S': //-y
			
			movey = movey - 20.0;
			
			if(movey < -((height - (300 * zoom)) / 2))
				movey = -((height - (300 * zoom)) / 2);
			else {//begins
			
				yT1 = yT1 + 20.0 * yTextFact;
				yT2 = yT2 + 20.0 * yTextFact;
			
			}//else ends
				
			movexd = movey * sin(theta) + movex * cos(theta);
			moveyd = movey * cos(theta) - 1 * movex * sin(theta); //change
			
			
				
			break;
			
			
			
		case 'w':
		case 'W':  //+y
		
			moveyLast = movey;
			movey = movey + 20.0;
			
			if(movey > ((height - (300 * zoom)) / 2))
				movey = ((height - (300 * zoom)) / 2);
			else {//begins
			
				yT1 = yT1 - 20.0 * yTextFact;
				yT2 = yT2 - 20.0 * yTextFact;
			
			}//else ends
				
			movexd = movey * sin(theta) + movex * cos(theta);
			moveyd = movey * cos(theta) - 1 * movex * sin(theta);
				
			break;
				
		case 'd':
		case 'D':  //-x
		
			movexLast = movex;
			movex = movex + 20.0;
			
			if(movex > ((width - (300 * zoom)) / 2))
				movex = ((width - (300 * zoom)) / 2);
			else {//begins
			
				xT1 = xT1 - 20.0 * xTextFact;
				xT2 = xT2 - 20.0 * xTextFact;
			
			}//else ends
				
			movexd = movex * cos(theta) + movey * sin(theta); //change
			moveyd = -1 * movex * sin(theta) + movey * cos(theta); 
				
			break;
			
		case 'a':
		case 'A':  //+x
		
			movexLast = movex;
			movex = movex - 20.0;
			
			if(movex < -((width - (300 * zoom)) / 2))
				movex = -((width - (300 * zoom)) / 2);
			else {//begins
			
				xT1 = xT1 + 20.0 * xTextFact;
				xT2 = xT2 + 20.0 * xTextFact;
			
			}//else ends
				
			movexd = movex * cos(theta) + movey * sin(theta); //change
			moveyd = -1 * movex * sin(theta) + movey * cos(theta); 
				
			
				
			break;

		case 'p':
		case 'P':			

		
			zoom = zoom + 0.5;
			
			if(zoom > 3.0)
				zoom = 3.0;			
				
			break;
			
		case 'o':
		case 'O':
		
			zoom = zoom - 0.5;
			
			if(zoom < 1.0)
				zoom = 1.0;
				
			break;

		case 'l':
		case 'L':			
		
			spin =  spin - 10.0 ;
	
			if(spin < 0.0)
				spin = spin + 360.0;
				
			theta = spin * PI / 180;
				
			movexd = movex * cos(theta) + movey * sin(theta); //change
			moveyd = -1 * movex * sin(theta) + movey * cos(theta); 
				
			break;
			
		case 'k':
		case 'K':
		

			spin =  spin + 10.0 ;
	
			if(spin > 360.0)
				spin = spin - 360.0;
				
			theta = spin * PI / 180;	
				
			movexd = movex * cos(theta) + movey * sin(theta); //change
			moveyd = -1 * movex * sin(theta) + movey * cos(theta); 			
							
			break;
			
	
	}//switch ends	
	
	theta = spin * PI / 180;	

}//ends

/*
 * Request double buffer display mode
 * Register mouse input callback functions
 */
int main(int argc, char** argv) {//begins

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Proyecto PDI 01: Camara de Tuberia");
	Init();
	Initialize_OpenCV();
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutIdleFunc(SpinDisplay);
	glutMainLoop();
	
	return 0;

}//main function ends
