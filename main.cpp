/*
 *First person view of a room implemented using ray casting.
 *Author:	Damir Hodzic
 *Date:		02.02.2019
 */

#include <Windows.h>
#include <iostream>
#include <string>
#include <math.h>
#include <conio.h>

#define PI 3.14159265
#define SCREENWIDTH 120
#define SCREENHEIGHT 40
#define WALLHEIGHT 1
#define WALKINGSPEED 0.1


double initialAngle = 0.0, cameraAngle = 0.00, fieldOfView = 60.0;
double absoluteRayAngle = 0.0, relativeRayAngle = 0.0; //relative to cameraAngle
double rayVector[2] = { 0,0 };
double baseVector[2] = { 0.01,0 };
double playerPosition[2] = { 8,8 };
double wallDistances[SCREENWIDTH] = { 0 };

wchar_t screenToDraw[SCREENHEIGHT][SCREENWIDTH];

char playMap[10][10] = {
	{ 1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,1,1,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,1 },
	{ 1,1,1,1,1,1,1,1,1,1 }
};


/*Window related variables for WINAPI*/
HANDLE std_out;
HANDLE consoleHandle;
char *error_msg;
COORD coordOrigin;
COORD coordMap;


/*********************
Function declarations
**********************
*/

/**
Lets the player rotate

@param direction The rotation direction: -1 left, 1 right
*/
void rotate(int direction);

/**
Handles all input
*/
void handleInput();

/**
Implements ray casting. All rays are calculated and saved into wallDistances array
*/
void calculateDistance();

/**
Fills the screenToDraw with unicode characters that are to be displayed later
*/
void fillScreen();

/**
Draws to the console the screenToDraw which has been filled with fillScreen before 
*/
void drawScreen();

/**
Initializes and creates the console window 
@return 0 on success, any other number on failure
*/
int initWindow();


int main() {
	if (initWindow() != 0)
		return -1;
		
	coordOrigin.X = 0;
	coordOrigin.Y = 0;
	coordMap.X = SCREENWIDTH - 10;
	coordMap.Y = SCREENHEIGHT - 10;
	cameraAngle = initialAngle;
	
	//gameloop
	while (1) {
		handleInput();
		calculateDistance();
		fillScreen();
		drawScreen();
		//Sleep(100);

	}
}


void handleInput() {
	char keyInput = ' ';

	if (_kbhit()) {
		keyInput = _getch();
		switch (keyInput) {
		case 'w':
			if (cameraAngle < 45 * PI / 180 || cameraAngle > 315 * PI / 180) {
				if (playerPosition[0] < 8)
					playerPosition[0] += WALKINGSPEED;
			}
			else if (cameraAngle > 45 * PI / 180 && cameraAngle < 135 * PI / 180) {
				if (playerPosition[1] > 1)
					playerPosition[1] -= WALKINGSPEED;
			}
			else if (cameraAngle > 135 * PI / 180 && cameraAngle <= 225 * PI / 180) {
				if (playerPosition[0] > 1)
					playerPosition[0] -= WALKINGSPEED;
			}
			else if (cameraAngle > 225 * PI / 180 && cameraAngle <= 315 * PI / 180) {
				if (playerPosition[1] < 8)
					playerPosition[1] += WALKINGSPEED;
			}
			break;
		case 's':
			if (cameraAngle < 45 * PI / 180 || cameraAngle > 315 * PI / 180) {
				if (playerPosition[0] > 1)
					playerPosition[0] -= WALKINGSPEED;
			}
			else if (cameraAngle > 45 * PI / 180 && cameraAngle < 135 * PI / 180) {
				if (playerPosition[1] < 8)
					playerPosition[1] += WALKINGSPEED;
			}
			else if (cameraAngle > 135 * PI / 180 && cameraAngle <= 225 * PI / 180) {
				if (playerPosition[0] < 8)
					playerPosition[0] += WALKINGSPEED;
			}
			else if (cameraAngle > 225 * PI / 180 && cameraAngle <= 315 * PI / 180) {
				if (playerPosition[1] > 1)
					playerPosition[1] -= WALKINGSPEED;
			}
			break;
		case 'a': rotate(1); break;
		case 'd': rotate(-1); break;
		case 'e': fieldOfView += 10; break;
		case 'q': fieldOfView -= 10;
		}
		keyInput = ' ';
	}

}

void rotate(int direction) {
	if (cameraAngle >= 360 * PI / 180 && direction == 1) {
		cameraAngle = 0;
		cameraAngle += direction*(initialAngle + (10))*PI / 180;
	}
	else if ((cameraAngle - (20 * PI / 180) < 0) && direction == -1) {
		cameraAngle = 360 * PI / 180;
		cameraAngle += direction*(initialAngle + (10))*PI / 180;
	}
	else cameraAngle += direction*(initialAngle + (10))*PI / 180;
	rayVector[0] = cos(cameraAngle)*baseVector[0] - sin(cameraAngle)*baseVector[1];
	rayVector[1] = sin(cameraAngle)*baseVector[0] + cos(cameraAngle)*baseVector[1];
}


void calculateDistance() {
	memset(&screenToDraw[0], 0, sizeof(screenToDraw));
	memset(&wallDistances[0], 0, sizeof(wallDistances));
	double rayVectorUnitX = 0.0;
	double rayVectorUnitY = 0.0;
	double x = 0.0, y = 0.0;
	bool found = false;

	int distanceCount = 0;
	relativeRayAngle = 0.0;
	absoluteRayAngle = 0 - fieldOfView;
	for(int i = 0; i < SCREENWIDTH; i++){
		relativeRayAngle = (initialAngle + (cameraAngle + (absoluteRayAngle*PI / 180)));

		rayVector[0] = cos(relativeRayAngle)*baseVector[0] - sin(relativeRayAngle)*baseVector[1];
		rayVector[1] = sin(relativeRayAngle)*baseVector[0] + cos(relativeRayAngle)*baseVector[1];
		rayVectorUnitX = rayVector[0];
		rayVectorUnitY = rayVector[1];
		while (!found) {
			x = rayVector[0] + playerPosition[0] + 0.5;
			y = (rayVector[1] * (-1)) + playerPosition[1] + 0.5;

			if (playMap[(int)x][(int)y] == 1) {
				wallDistances[distanceCount] = sqrt(pow(rayVector[0], 2.0) + pow(rayVector[1], 2.0))*cos(absoluteRayAngle*PI / 180);
				//distance[distanceCount] = (y / sin(rayAngle))*cos(rayAngle);
				found = true;
				distanceCount++;
			}
			else {
				rayVector[0] += rayVectorUnitX;
				rayVector[1] += rayVectorUnitY;
			}
		}
		found = false;
		absoluteRayAngle += (fieldOfView * 2 / SCREENWIDTH);

	}

}

void fillScreen() {
	double viewSize = 0.0;
	int start;
	int objectSizeOnScreen = 0;
	for (int i = 0; i < SCREENWIDTH; i++) {
		viewSize = wallDistances[SCREENWIDTH - i] * tan((double)60 * PI / 180) * 2;
		objectSizeOnScreen = WALLHEIGHT / wallDistances[SCREENWIDTH - i] * (((double)SCREENWIDTH / 2.0) / tan(60.0*PI / 180.0));

		if (objectSizeOnScreen > (SCREENHEIGHT - 1)) objectSizeOnScreen = SCREENHEIGHT;
		start = (SCREENHEIGHT - objectSizeOnScreen) / 2;

		start = start;
		for (int j = 0; j < objectSizeOnScreen; j++) {
			if (wallDistances[SCREENWIDTH - i] > 7)
				screenToDraw[j + start][i] = L'░';
			else if (wallDistances[SCREENWIDTH - i] > 4)

				screenToDraw[j + start][i] = L'▒';
			else if (wallDistances[SCREENWIDTH - i] > 2.5)
				screenToDraw[j + start][i] = L'▓';
			else if (wallDistances[SCREENWIDTH - i] > 0) screenToDraw[j + start][i] = L'█';
		}
	}
	for (int i = coordMap.Y; i < (coordMap.Y + 10); i++) {
		for (int j = (int)coordMap.X; j < (coordMap.X + 10); j++) {
			if (playMap[i - coordMap.Y][j - coordMap.X] == 1)
				screenToDraw[i][j] = L'#';
			else screenToDraw[i][j] = L' ';

		}
	}
	screenToDraw[coordMap.Y + (int)playerPosition[1]][coordMap.X + (int)playerPosition[0]] = L'P';
}

void drawScreen() {
	SetConsoleCursorPosition(consoleHandle, coordOrigin);
	bool test = WriteConsoleW(consoleHandle, &screenToDraw, SCREENWIDTH*SCREENHEIGHT, 0, NULL);


	wchar_t anglew[5];

	swprintf_s(anglew, L"%d", (int)(cameraAngle * 180 / PI));
	WriteConsoleW(consoleHandle, anglew, 3, 0, NULL);

}

int initWindow() {
	consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	COORD coordScreenBuffer = { SCREENWIDTH, SCREENHEIGHT + 1 };
	SMALL_RECT windowSize = { 0, 0, SCREENWIDTH - 1, SCREENHEIGHT };

	if (consoleHandle == INVALID_HANDLE_VALUE) {
		return (GetLastError());
	}

	if (!SetConsoleActiveScreenBuffer(consoleHandle))
	{
		return (GetLastError());

	}

	if (!SetConsoleScreenBufferSize(consoleHandle, coordScreenBuffer)) {
		return (GetLastError());

	}

	if (!SetConsoleWindowInfo(consoleHandle, TRUE, &windowSize))
	{
		return (GetLastError());
	}

	return 0;
}
