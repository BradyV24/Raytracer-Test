#include <stdio.h>
#include <stdlib.h>
#include <gl/glut.h>
#include <math.h>

#define PI 3.1415926535
#define MWIDTH 1026
#define WWIDTH 700
#define HEIGHT 1026

typedef struct {
	int w, a, d, s;
}ButtonKeys; ButtonKeys Keys;

float px, py, pdx, pdy, pa; //player position

void drawPlayer() {
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px, py);
	glEnd();

	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px, py);
	glVertex2i(px + pdx * 5, py + pdy * 5);
	glEnd();
}

int mapX = 16, mapY = 16, mapS = 64;
int map[]=
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
	1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
	1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
	1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
	1,0,0,0,1,0,0,0,1,1,1,1,0,0,1,1,
	1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,
	1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,
	1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,
	1,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,
	1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

void drawMap2D() {
	int x, y, xo, yo;
	for (y = 0; y < mapY; y++) {
		for (x = 0; x < mapX; x++) {
			if (map[y*mapX + x] == 1) glColor3f(0.15, 0.35, 0.15);
			else glColor3f(0.55, 0.45, 0.25);
			xo = x * mapS; yo = y * mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo + 1, yo + 1);
			glVertex2i(xo + 1, yo + mapS - 1);
			glVertex2i(xo + mapS - 1, yo + mapS - 1);
			glVertex2i(xo + mapS - 1, yo + 1);
			glEnd();
		}
	}
}

float dist(float ax, float ay, float bx, float by, float ang) {
	return (sqrt(pow(bx - ax, 2) + pow(by - ay, 2)));
}

void drawRays3D() {

	float fov = 60.;

	int r, mx, my, mp, dof; float rx, ry, ra, xo, yo,disT;
	ra = pa - fov/2 * PI/180;
	if (ra < 0) ra += 2 * PI;
	for (r = 0; r < WWIDTH; r++, ra+=fov/WWIDTH * PI/180) {
		if (ra > 2*PI) ra -= 2 * PI;
		//Horizontal Lines check
		dof = 0;
		float disH = 10000, hx, hy;
		float aTan = -1 / tan(ra);
		if (ra > PI) { ry = (((int)py >> 6) << 6) - 0.0001; rx = (py - ry)*aTan + px; yo = -64; xo = -yo * aTan; } //Looking up
		if (ra < PI) { ry = (((int)py >> 6) << 6) + 64; rx = (py - ry)*aTan + px; yo = 64; xo = -yo * aTan; } //Looking down
		if (ra == 0 || ra == PI) { rx = px; ry = py; dof = 16; } //Looking along x-axis
		while (dof < 16) {
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
			if (mp > 0 && mp < mapX*mapY && map[mp] == 1) { hx = rx; hy = ry; disH = dist(px, py, hx, hy, ra); dof = 16; }//hit wall
			else { rx += xo; ry += yo; dof += 1; } //next check
		}

		//Vertical Lines check
		dof = 0;
		float disV = 10000, vx, vy;
		float nTan = -tan(ra);
		if (ra > PI / 2 && ra < 3 * PI / 2) { rx = (((int)px >> 6) << 6) - 0.0001; ry = (px - rx)*nTan + py - 0.0001;  xo = -64; yo = -xo * nTan; } //Looking left
		if (ra < PI / 2 || ra > 3 * PI / 2) { rx = (((int)px >> 6) << 6) + 64; ry = (px - rx)*nTan + py - 0.0001;  xo = 64; yo = -xo * nTan; } //Looking right
		if (ra == PI || ra == 0) { rx = px; ry = py; dof = 16; } //Looking along y-axis
		while (dof < 16) {
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
			if (mp > 0 && mp < mapX*mapY && map[mp] == 1) { vx = rx; vy = ry; disV = dist(px, py, vx, vy, ra); dof = 16;}//hit wall
			else { rx += xo; ry += yo; dof += 1; } //next check
		}

		if (disV < disH) { rx = vx; ry = vy; disT = disV; glColor3f(0.15, 0.35, 0.15);}
		if (disH < disV) { rx = hx; ry = hy; disT = disH; glColor3f(0.25, 0.45, 0.0);}
		//2D rays
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(px, py);
		glVertex2i(rx, ry);
		glEnd();

		//Draw Walls
		float ca = pa - ra; if (ca < 0) ca += 2 * PI; if (ca > 2 * PI) ca -= 2 * PI; disT = disT * cos(ca);
		float lineH = (mapS*HEIGHT / disT); if (lineH > HEIGHT) { lineH = HEIGHT; };
		glLineWidth(1); 
		glBegin(GL_LINES);
		glVertex2i(r + MWIDTH, HEIGHT / 2 - lineH / 2);
		glVertex2i(r + MWIDTH, HEIGHT / 2 + lineH / 2);
		glEnd();
	}
}

float frame1, frame2, fps;

void display() {
	//fps
	frame2 = glutGet(GLUT_ELAPSED_TIME);
	fps = (frame2 - frame1);
	frame1 = glutGet(GLUT_ELAPSED_TIME);

	//buttons and collision
	if (Keys.a == 1) { pa -= 0.004*fps; if (pa < 0) { pa += 2 * PI; } pdx = cos(pa) * 5; pdy = sin(pa) * 5; }
	if (Keys.d == 1) { pa += 0.004*fps; if (pa > 2 * PI) { pa -= 2 * PI; } pdx = cos(pa) * 5; pdy = sin(pa) * 5; }
	
	int xo = 0; if (pdx < 0) xo = -20; else xo = 20;
	int yo = 0; if (pdy < 0) yo = -20; else yo = 20;
	int ipx = px / 64., ipx_add_xo = (px + xo) / 64., ipx_sub_xo = (px - xo) / 64.;
	int ipy = py / 64., ipy_add_yo = (py + yo) / 64., ipy_sub_yo = (py - yo) / 64.;

	if (Keys.w == 1) {
		if (map[ipy*mapX + ipx_add_xo] == 0) px += pdx * fps*0.05;
		if (map[ipy_add_yo*mapX + ipx] == 0) py += pdy * fps*0.05;
	   }
	if (Keys.s == 1) {
		if (map[ipy*mapX + ipx_sub_xo] == 0) px -= pdx * fps*0.05;
		if (map[ipy_sub_yo*mapX + ipx] == 0) py -= pdy * fps*0.05;
	}
	
	glutPostRedisplay();




	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw Sky
	glColor3f(0.3, 0.5, 0.7);
	glBegin(GL_QUADS);
	glVertex2i(0 + MWIDTH, 0);
	glVertex2i(WWIDTH + MWIDTH, 0);
	glVertex2i(WWIDTH + MWIDTH, HEIGHT / 2);
	glVertex2i(0 + MWIDTH, HEIGHT / 2);
	glEnd();

	drawMap2D();
	drawPlayer();
	drawRays3D();
	glutSwapBuffers();
}

void resize(int w, int h) {
	glutReshapeWindow(WWIDTH + MWIDTH, HEIGHT);
}

void init() {
	glClearColor(0.5, 0.4, 0.2, 0);
	gluOrtho2D(0, WWIDTH+MWIDTH, HEIGHT, 0);
	px = 300; py = 300; pdx = cos(pa) * 5; pdy = sin(pa) * 5;
}

void ButtonDown(unsigned char key, int x, int y) {
	if (key == 'a') Keys.a = 1;
	if (key == 'd') Keys.d = 1;
	if (key == 'w') Keys.w = 1;
	if (key == 's') Keys.s = 1;
	glutPostRedisplay();
}

void ButtonUp(unsigned char key, int x, int y) {
	if (key == 'a') Keys.a = 0;
	if (key == 'd') Keys.d = 0;
	if (key == 'w') Keys.w = 0;
	if (key == 's') Keys.s = 0;
	glutPostRedisplay();
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WWIDTH + MWIDTH, HEIGHT);
	glutCreateWindow("OpenGL Test 1: LEARNING THE BASICS");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(ButtonDown);
	glutKeyboardUpFunc(ButtonUp);
	glutMainLoop();
}