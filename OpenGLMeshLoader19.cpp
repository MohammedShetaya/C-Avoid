#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <cmath>
#include <iostream>
#include<iostream>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <MMSystem.h>
#include <mciapi.h>
#include <stdlib.h>
#include <glut.h>
#include<algorithm>
#include <math.h>
#include <random>
#include <Mmsystem.h>
#include <mciapi.h>
#include <playsoundapi.h>
#include <chrono>
#include <thread>

#pragma comment(lib,"Winmm.lib")

using namespace std;

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex_sand_sky, tex_ice_sky;
char title[] = "Corona Subway";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 300;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

int FirstEyeX;
int FirstEyeY;
int FirstEyeZ;
int ThirdEyeX;
int ThirdEyeY;
int ThirdEyeZ;
int BackEyeX;
int BackEyeY;
int BackEyeZ;

int FirstAtX;
int FirstAtY;
int FirstAtZ;
int ThirdAtX;
int ThirdAtY;
int ThirdAtZ;
int BackAtX;
int BackAtY;
int BackAtZ;

int light = 0;

float playerX = 0;

float playerRotate = 0;
bool playerRot = false;

int goldRot = 0;

int playerFrame = 5 ;
int frameDirection = 1 ;
int freq = 10 ; 

Vector Eye(0, 0, 0);
Vector At(0, 0, 0);
Vector Up(0, 0, 1);

int cameraZoom = 0;

// Model Variables
Model_3DS model_building_sand_1;
Model_3DS model_building_sand_2;
Model_3DS model_building_ice_1;
Model_3DS model_building_ice_2;
Model_3DS model_barrier;
Model_3DS model_gold;
Model_3DS model_drink;
vector<Model_3DS> model_character;

// Textures
GLTexture tex_ground_sand;
GLTexture tex_ground_ice;

// Functions
void timer(int);
void print(int x, int y, char* string);
void printT(int x, int y, int z, char* string);
void setPositions();

// Game variables
float totalDistance;
float preDistance;
float postDistance;
float halfRoadWidth;
float sideRoadBoundary;
float curDistance;
int level;
int lives;
int score;
bool waitScreen;
bool gameEndedLose;
bool waitLevelTwo;
bool gameEndedWin;
vector<int> rightBuildings;
vector<int> rightBuildingsType;
vector<int> leftBuildings;
vector<int> leftBuildingsType;
vector<int> barriers;

vector<pair<int, bool>> rightBarriers;
vector<pair<int, bool>> leftBarriers;

vector<pair<pair<int, int>, bool>> goldPos;


vector<pair<int, bool>> drinkPos;


//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 20.0f, 20.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat light_direction[] = { 0.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}


void RenderGround()
{

	glColor3f(1, 1, 1);
	glDisable(GL_LIGHTING);	// Disable lighting 
	glEnable(GL_TEXTURE_2D); // Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, level == 2 ? tex_ground_ice.texture[0] : tex_ground_sand.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);
	glVertex3f(halfRoadWidth, -preDistance - curDistance, 0);
	glTexCoord2f(100, 0);
	glVertex3f(-halfRoadWidth, -preDistance - curDistance, 0);
	glTexCoord2f(100, 100);
	glVertex3f(-halfRoadWidth, totalDistance - curDistance, 0);
	glTexCoord2f(0, 100);
	glVertex3f(halfRoadWidth, totalDistance - curDistance, 0);
	glEnd();
	glPopMatrix();

	glBegin(GL_QUADS);
	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);
	glVertex3f(sideRoadBoundary, -preDistance - curDistance, 0);
	glTexCoord2f(100, 0);
	glVertex3f(halfRoadWidth, -preDistance - curDistance, 0);
	glTexCoord2f(100, 100);
	glVertex3f(halfRoadWidth, totalDistance - curDistance, 0);
	glTexCoord2f(0, 100);
	glVertex3f(sideRoadBoundary, totalDistance - curDistance, 0);
	glEnd();
	glPopMatrix();

	glBegin(GL_QUADS);
	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);
	glVertex3f(-halfRoadWidth, -preDistance - curDistance, 0);
	glTexCoord2f(100, 0);
	glVertex3f(-sideRoadBoundary, -preDistance - curDistance, 0);
	glTexCoord2f(100, 100);
	glVertex3f(-sideRoadBoundary, totalDistance - curDistance, 0);
	glTexCoord2f(0, 100);
	glVertex3f(-halfRoadWidth, totalDistance - curDistance, 0);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}


void goldCollision() {
	for (int i = 0; i < goldPos.size(); i++) {
		if (abs(playerX - goldPos[i].first.first) < 1.5 && abs((goldPos[i].first.second - curDistance)) < 1 && !goldPos[i].second) {
			goldPos[i].second = true;
			score += 5;
			sndPlaySound(TEXT("gamemusic/gold.wav"), SND_ASYNC | SND_FILENAME);
		}
	}
	for (int i = 0; i < drinkPos.size(); i++) {
		if (abs(playerX) < 1.5 && abs((drinkPos[i].first - curDistance)) < 1 && drinkPos[i].second == false) {
			drinkPos[i].second = true;
			sndPlaySound(TEXT("gamemusic/drink.wav"), SND_ASYNC | SND_FILENAME);
			lives += 1;
		}
	}

	for (int i = 0; i < rightBarriers.size(); i++) {
		if (playerX > -1 && abs((rightBarriers[i].first - curDistance)) < 1 && rightBarriers[i].second == false) {
			sndPlaySound(TEXT("gamemusic/hit.wav"), SND_ASYNC | SND_FILENAME);
			rightBarriers[i].second = true;
			lives -= 1;
		}
	}

	for (int i = 0; i < leftBarriers.size(); i++) {

		if (playerX < 1 && abs((leftBarriers[i].first - curDistance)) < 1 && leftBarriers[i].second == false) {
			sndPlaySound(TEXT("gamemusic/hit.wav"), SND_ASYNC | SND_FILENAME);
			leftBarriers[i].second = true;
			lives -= 1;
		}
	}
}
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myInit();

	if (waitScreen) {
		glPushMatrix();
		glClearColor(1.0, 1.0, 1.0, 1.0);
		if (gameEndedLose) {
			sndPlaySound(TEXT("gamemusic/gameover.wav"), SND_ASYNC | SND_FILENAME);
			char* endScore[50];
			glColor3f(1, 1, 1);
			sprintf((char*)endScore, "Game Ended! You LOST!");
			printT(0, 5, 5, (char*)endScore);

			char* endText[50];
			sprintf((char*)endText, "Press any button to start again");
			printT(0, 5 + 1, 5 + 1, (char*)endText);
		}
		else if (waitLevelTwo) {
			char* endScore[50];
			glColor3f(1, 1, 1);
			sndPlaySound(TEXT("gamemusic/win.wav"), SND_ASYNC | SND_FILENAME);
			sprintf((char*)endScore, "You finished level 1");
			printT(0, 5, 5, (char*)endScore);

			char* endText[50];
			sprintf((char*)endText, "Press any button to start Leve 2");
			printT(0, 5 + 1, 5 + 1, (char*)endText);
		}
		else if (gameEndedWin) {
			sndPlaySound(TEXT("gamemusic/win.wav"), SND_ASYNC | SND_FILENAME);
			char* endScore[50];
			glColor3f(1, 1, 1);
			sprintf((char*)endScore, "Game ended with score %d", score);
			printT(0, 5, 5, (char*)endScore);

			char* endText[50];
			sprintf((char*)endText, "Press any button to play again");
			printT(0, 5 + 1, 5 + 1, (char*)endText);
		}
		glPopMatrix();
	}
	else {
		if (light == 0) {
			GLfloat lightIntensity[] = { 1.0, 1.0 ,1.0, 1.0f };
			GLfloat lightPosition[] = { 0.0f,0.0f, 0.0f, 0.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
		}

		if (light == 1) {
			GLfloat lightIntensity[] = { 1.0, 0.5 ,0.5, 1.0f };
			GLfloat lightPosition[] = { 20.0f, 0.0f,0.0f, 0.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
		}
		if (light == 2) {
			GLfloat lightIntensity[] = { 0.5, 1.0 ,0.5, 1.0f };
			GLfloat lightPosition[] = { -20.0f, 0.0f, 0.0f, 0.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
		}

		// Draw Score, Live, Level
		glPushMatrix();
		if (level == 1)
			glColor3f(0, 0, 0);
		char* endScore[50];
		sprintf((char*)endScore, "Score: %d", score);
		printT(-5, 13, 13, (char*)endScore);

		char* endText[50];
		sprintf((char*)endText, "Lives Remaining: %d", lives);
		printT(-5, 12, 12, (char*)endText);

		char* endText2[50];
		sprintf((char*)endText2, "Level: %d", level);
		printT(-5, 11, 11, (char*)endText2);
		glPopMatrix();


		// Draw Ground
		RenderGround();

		//draw sky
		glDisable(GL_LIGHTING);
		glPushMatrix();
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, level == 2 ? tex_ice_sky : tex_sand_sky);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 120, 120, 120);
		gluDeleteQuadric(qobj);
		glPopMatrix();
		glEnable(GL_LIGHTING);



		// draw scooter
		glPushMatrix();
		glTranslated(playerX, 0, 2);
		glRotatef(playerRotate, 0, 1, 0);
		glRotatef(180.f, 0, 1, 0);
		glScalef(3, 3, 3);
		model_character[playerFrame].Draw();
		glPopMatrix();


		//printf("\n %f", curDistance);

		//Draw barriers
		for (int i = 0; i < rightBarriers.size(); i++) {
			if (!rightBarriers[i].second) {
				glPushMatrix();
				glTranslatef(9, rightBarriers[i].first - curDistance, 0);
				glRotatef(180, 1, 0, 0);
				glScalef(0.09, 0.05, 0.03);
				model_barrier.Draw();
				glPopMatrix();
			}
		}
		for (int i = 0; i < leftBarriers.size(); i++) {
			if (!leftBarriers[i].second) {
				glPushMatrix();
				glTranslatef(-9, leftBarriers[i].first - curDistance, 0);
				glRotatef(180, 1, 0, 0);
				glScalef(0.09, 0.05, 0.03);
				model_barrier.Draw();
				glPopMatrix();
			}
		}

		// draw gold
		for (int i = 0; i < goldPos.size(); i++) {
			if (goldPos[i].second == false) {
				glPushMatrix();
				glTranslatef(goldPos[i].first.first, goldPos[i].first.second - curDistance, 2);
				glRotatef(180, 1, 0, 0);
				glRotatef(goldRot, 0, 0, 1);
				glScalef(2, 2, 2);
				model_gold.Draw();
				glPopMatrix();
			}

		}
		goldCollision();

		// draw drink
		for (int i = 0; i < drinkPos.size(); i++) {
			if (drinkPos[i].second == false) {
				glPushMatrix();
				glTranslatef(0, drinkPos[i].first - curDistance, 3);
				glRotatef(180, 1, 0, 0);
				glRotatef(goldRot, 0, 0, 1);
				glScalef(0.5, 0.5, 0.5);
				model_drink.Draw();
				glPopMatrix();
			}
		}
		// Draw buildings
		for (int i = 0; i < rightBuildings.size(); i++) {
			int pos = rightBuildings[i];
			int type = rightBuildingsType[i];
			glPushMatrix();
			glTranslatef(-30, pos - curDistance, 0);
			glRotatef(180.f, 1, 0, 0);
			glScalef(0.2, 0.2, 0.2);
			if (level == 1) {
				if (type == 0)
					model_building_sand_1.Draw();
				else
					model_building_sand_2.Draw();
			}
			else {
				if (type == 0)
					model_building_ice_1.Draw();
				else
					model_building_ice_2.Draw();
			}
			glPopMatrix();
		}

		for (int i = 0; i < leftBuildings.size(); i++) {
			int pos = leftBuildings[i];
			int type = leftBuildingsType[i];
			glPushMatrix();
			glTranslatef(30, pos - curDistance, 0);
			glRotatef(180.f, 1, 0, 0);
			glScalef(0.2, 0.2, 0.2);
			if (level == 1) {
				if (type == 0)
					model_building_sand_1.Draw();
				else
					model_building_sand_2.Draw();
			}
			else {
				if (type == 0)
					model_building_ice_1.Draw();
				else
					model_building_ice_2.Draw();
			}
			glPopMatrix();
		}


	}

	glutSwapBuffers();
}

void myKeyboard(unsigned char button, int x, int y)
{
	if (waitScreen) {
		waitScreen = false;
		if (gameEndedLose) {
			setPositions();
			gameEndedLose = false;
			curDistance = 0;
			level = 1;
			lives = 3;
			score = 0;
		}
		else if (waitLevelTwo) {
			setPositions();
			waitLevelTwo = false;
			curDistance = 0;
			level = 2;
		}
		else {
			setPositions();
			gameEndedWin = false;
			curDistance = 0;
			level = 1;
			lives = 3;
			score = 0;
		}
		glutPostRedisplay();
		glutTimerFunc(0, timer, 0);
	}
	else {
		switch (button)
		{
		case 'i':
			light = 0;
			break;
		case 'o':
			light = 1;
			break;
		case 'p':
			light = 2;
			break;
		case 'l':
			Eye.x = playerX;
			Eye.y = FirstEyeY;
			Eye.z = FirstEyeZ;
			At.x = playerX;
			At.y = FirstAtY;
			At.z = FirstAtZ;
			break;
		case 'k':
			Eye.x = ThirdEyeX;
			Eye.y = ThirdEyeY;
			Eye.z = ThirdEyeZ;
			At.x = ThirdAtX;
			At.y = ThirdAtY;
			At.z = ThirdAtZ;
			break;
		case 'j':
			Eye.x = BackEyeX;
			Eye.y = BackEyeY;
			Eye.z = BackEyeZ;
			At.x = BackAtX;
			At.y = BackAtY;
			At.z = BackAtZ;
			break;
		case '4':
			if (playerX > -5)
				playerX -= 2;
			break;
		case '6':
			if (playerX < 5)
				playerX += 2;
			break;
		default:

			break;
		}
		glutPostRedisplay();
	}
}

void myKeyUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'j':
		Eye.x = ThirdEyeX;
		Eye.y = ThirdEyeY;
		Eye.z = ThirdEyeZ;
		At.x = ThirdAtX;
		At.y = ThirdAtY;
		At.z = ThirdAtZ;
		break;
	case '4':
		playerRotate = 0;
		break;
	case '6':
		playerRotate = 0;
		break;
	}

	glutPostRedisplay();
}

void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
		Eye.y += -0.1;
	else
		Eye.y += 0.1;

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters


	glutPostRedisplay();	//Re-draw scene
}

void myMouse(int button, int state, int x, int y)
{
	//y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

void timer(int) {
	// do changes here
	curDistance += 0.7;
	//score++;
	goldRot++;

	glutPostRedisplay();
	// when game ends, return here
	if (lives == 0) {
		waitScreen = true;
		gameEndedLose = true;
		return;
	}
	else if (curDistance >= totalDistance - postDistance) {
		waitScreen = true;

		if (level == 1) {
			waitLevelTwo = true;
		}
		else {
			gameEndedWin = true;
		}
		return;
	}

	if (freq == 0)
	{
		freq = 2 ;
		playerFrame += frameDirection;

		if (playerFrame == 0 || playerFrame == model_character.size() - 1)
		{
			frameDirection *= -1;
		}

	}
	
	freq--;

	glutTimerFunc(10, timer, 0);
}

/*void frameTimer(int) {
	if (playerFrame == 10 || playerFrame == 0) {
		frameDirection *= -1; 
	}

	playerFrame += frameDirection;

	glutTimerFunc(200 , timer, 0);
}*/

void LoadAssets()
{
	// Loading Model files
	//model_house.Load("Models/house/house.3DS");
	model_building_sand_1.Load("Models/buildings/building_sand_1/building.3DS");
	model_building_sand_2.Load("Models/buildings/building_sand_2/building.3DS");
	model_building_ice_1.Load("Models/buildings/building_ice_1/building.3DS");
	model_building_ice_2.Load("Models/buildings/building_ice_2/building.3DS");
	//model_scooter.Load("Models/scooter/scooter.3DS");
	model_barrier.Load("Models/barrier/barrier.3DS");
	model_gold.Load("Models/gold/gold.3DS");
	model_drink.Load("Models/drink/drink.3DS");
	//model_tree.Load("Models/tree/Tree1.3ds");
	Model_3DS c_6;
	c_6.Load("Models/spartan/-6.3DS");
	Model_3DS c_5;
	c_5.Load("Models/spartan/-5.3DS");
	Model_3DS c_4;
	c_4.Load("Models/spartan/-4.3DS");
	Model_3DS c_3;
	c_3.Load("Models/spartan/-3.3DS");
	Model_3DS c_2;
	c_2.Load("Models/spartan/-2.3DS");
	Model_3DS c_1;
	c_1.Load("Models/spartan/-1.3DS");
	Model_3DS c1;
	c1.Load("Models/spartan/1.3DS");
	Model_3DS c2;
	c2.Load("Models/spartan/2.3DS");
	Model_3DS c3;
	c3.Load("Models/spartan/3.3DS");
	Model_3DS c4;
	c4.Load("Models/spartan/4.3DS");
	Model_3DS c5;
	c5.Load("Models/spartan/5.3DS");
	model_character.push_back(c_6);
	model_character.push_back(c_5);
	model_character.push_back(c_4);
	model_character.push_back(c_3);
	model_character.push_back(c_2);
	model_character.push_back(c_1);
	model_character.push_back(c1);
	model_character.push_back(c2);
	model_character.push_back(c3);
	model_character.push_back(c4);

	// Loading texture files
	tex_ground_sand.Load("Textures/sand.bmp");
	tex_ground_ice.Load("Textures/ice.bmp");
	loadBMP(&tex_sand_sky, "Textures/sand_sky.bmp", true);
	loadBMP(&tex_ice_sky, "Textures/ice_sky.bmp", true);
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutKeyboardUpFunc(myKeyUp);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutTimerFunc(0, timer, 0);

//	glutTimerFunc(0, frameTimer, 0);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	// initialize variables here
	curDistance = 0;
	level = 1;
	lives = 3;
	score = 0;
	waitScreen = false;
	gameEndedLose = false;
	waitLevelTwo = false;
	gameEndedWin = false;
	totalDistance = 1000;
	preDistance = 75;
	postDistance = 30;
	halfRoadWidth = 20;
	sideRoadBoundary = 200;

	Eye.x = ThirdEyeX = 0;
	Eye.y = ThirdEyeY = -15;
	Eye.z = ThirdEyeZ = 8;

	At.x = ThirdAtX = 0;
	At.y = ThirdAtY = 0;
	At.z = ThirdAtZ = 5;

	FirstEyeX = 0;
	FirstEyeY = 10;
	FirstEyeZ = 3;

	FirstAtX = 0;
	FirstAtY = 20;
	FirstAtZ = 3;

	BackEyeX = 0;
	BackEyeY = 10;
	BackEyeZ = 3;

	BackAtX = 0;
	BackAtY = 0;
	BackAtZ = 3;

	setPositions();

	glutMainLoop();
}

void setPositions() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(30, 70);
	std::uniform_int_distribution<std::mt19937::result_type> dist7(0, 1);
	std::uniform_int_distribution<std::mt19937::result_type> dist8(0, halfRoadWidth / 3);
	int cur = 0;
	rightBuildings.clear();
	rightBuildingsType.clear();
	leftBuildings.clear();
	leftBuildingsType.clear();
	rightBarriers.clear();
	leftBarriers.clear();
	drinkPos.clear();
	goldPos.clear();


	while (cur < totalDistance - 100) {
		rightBuildings.push_back(cur + dist6(rng));
		rightBuildingsType.push_back(dist7(rng));
		leftBuildings.push_back(cur + dist6(rng));
		leftBuildingsType.push_back(dist7(rng));
		cur += 100;
	}

	for (int i = 1; i * 79 < totalDistance - 30; i++) {
		int temp = dist7(rng);
		if (temp == 0) rightBarriers.push_back(make_pair(i * 79, false));
		else leftBarriers.push_back(make_pair(i * 79, false));
	}

	for (int i = 0; i * 23 < totalDistance - 30; i++) {
		int temp = (dist7(rng) == 0 ? dist8(rng) : -dist8(rng));
		for (int j = 0; j < 3; j++) {
			goldPos.push_back(make_pair(make_pair(temp, j * 5 + i * 23), false));
		}

	}

	for (int i = 0; i * 70 < totalDistance - 30; i++) {
		if (dist7(rng) == 1) {
			drinkPos.push_back(make_pair(i * 70, false));
		}
	}
}

void print(int x, int y, char* string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

void printT(int x, int y, int z, char* string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos3f(x, y, z);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}