#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#define   WIRE 0      
#define   SHADE 1      
#define   PI 3.141592
GLfloat light_diffuse[] = { 1.0, 0.8, 0.2, 1.0 };  /* Red diffuse light. */
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };  /* Infinite light location. */
GLfloat   light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; /* specular light */
GLfloat   light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };  /* ambient light */


typedef struct {
	float x;
	float y;
	float z;
} Point;

typedef struct {
	unsigned long ip[3];
} Face;

int pnum;
int fnum;
Point *mpoint;
Face *mface;

GLfloat angle1 = -150;   /* in degrees */
GLfloat angle2 = -150;   /* in degrees */
GLfloat light_angle = -150;   /* in degrees */

GLfloat xloc = 0, yloc = 0, zloc = 0;
int moving, beginx, beginy;
int light_moving;
float scalefactor = 1.0;
int scaling = 0;
int status = 0;           // WIRE or SHADE
int cull = 0;             // CULLING toggle 
char *fname = "cube.dat";

void DrawWire(void)
{
	//   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(1);
	glutSwapBuffers();
}

void DrawShade(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
	glutSwapBuffers();
}

Point cnormal(Point a, Point b, Point c)
{
	Point p, q, r;
	double val;
	p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z;
	q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z;

	r.x = p.y*q.z - p.z*q.y;
	r.y = p.z*q.x - p.x*q.z;
	r.z = p.x*q.y - p.y*q.x;

	val = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
	r.x = r.x / val;
	r.y = r.y / val;
	r.z = r.z / val;
	return r;
}

void MakeGL_Model(void)
{
	int i;
	Point norm;
	glShadeModel(GL_SMOOTH);

	glPushMatrix();
	glRotatef(light_angle, 0.0, 1.0, 0.0);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	if (glIsList(1)) glDeleteLists(1, 1);
	glNewList(1, GL_COMPILE);
	glPushMatrix();
	glTranslatef(xloc, yloc, zloc);
	glRotatef(angle1, 0.0, 1.0, 0.0); //y축 회전
	glRotatef(angle2, 1.0, 0.0, 0.0); //x축 회전
	glScalef(scalefactor, scalefactor, scalefactor);
	for (i = 0; i < fnum; i++) {
		norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]],
			mpoint[mface[i].ip[0]]);
		glBegin(GL_TRIANGLES);
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y,
			mpoint[mface[i].ip[0]].z);
		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y,
			mpoint[mface[i].ip[1]].z);
		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y,
			mpoint[mface[i].ip[2]].z);
		glEnd();
	}
	glPopMatrix();
	glEndList();
}

void GLSetupRC(void)
{
	/* Enable a single OpenGL light. */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);


	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(40.0, // field of view in degree 
		1.0, // aspect ratio 
		1.0, // Z near 
		2000.0); // Z far 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(400.0, 400.0, 400.0,  // eye is at (0,0,5) 눈의 위치 설정   
		0.0, 0.0, 0.0,      // center is at (0,0,0) 눈이 바라보는 지점      위치,지점가지고 N벡터 구함
		0.0, 1.0, 0.0);      // up is in positive Y direction  서서보는지 누워서보는지... V벡터 구함   N벡터와 V벡터로 U벡터구함
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MakeGL_Model();
	if (status == WIRE) DrawWire();
	else DrawShade();
}

void ReadModel()
{
	FILE *f1;
	char s[81];

	int i;

	if ((f1 = fopen(fname, "rt")) == NULL) {
		printf("No file\n");
		exit(0);
	}
	fscanf(f1, "%s", s);     printf("%s", s);
	fscanf(f1, "%s", s);     printf("%s", s);
	fscanf(f1, "%d", &pnum);     printf("%d\n", pnum);

	mpoint = (Point*)malloc(sizeof(Point)*pnum);

	for (i = 0; i<pnum; i++) {
		fscanf(f1, "%f", &mpoint[i].x);
		fscanf(f1, "%f", &mpoint[i].y);
		fscanf(f1, "%f", &mpoint[i].z);
		printf("%f %f %f\n", mpoint[i].x, mpoint[i].y, mpoint[i].z);
	}

	fscanf(f1, "%s", s);     printf("%s", s);
	fscanf(f1, "%s", s);     printf("%s", s);
	fscanf(f1, "%d", &fnum);     printf("%d\n", fnum);

	mface = (Face*)malloc(sizeof(Face)*fnum);
	for (i = 0; i<fnum; i++) {
		fscanf(f1, "%d", &mface[i].ip[0]);
		fscanf(f1, "%d", &mface[i].ip[1]);
		fscanf(f1, "%d", &mface[i].ip[2]);
		printf("%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
	}
	fclose(f1);
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		moving = 1;
		beginx = x;
		beginy = y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		moving = 0;
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		scaling = 1;
		beginx = x;
		beginy = y;
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
		scaling = 0;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		scaling = 0;
		light_moving = 1;
		beginx = x;
		beginy = y;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		light_moving = 0;
	}
}

void motion(int x, int y)
{
	if (scaling) {
		scalefactor = scalefactor * (1.0 + (beginx - x)*0.0001);
		glutPostRedisplay();
	}
	if (moving) {
		angle1 = angle1 + (x - beginx);
		angle2 = angle2 + (beginy - y); //방향이 반대라 위랑 반대로 빼줘야함
		beginx = x;
		beginy = y;
		glutPostRedisplay();
	}
	if (light_moving) {
		light_angle = light_angle + (x - beginx);
		beginx = x;
		beginy = y;
		glutPostRedisplay();
	}
}

void MakeSORModel()
{
	int n = 0;
	int dot = 15;
	int i, j = 0;
	int IncAngle = 15;         // 회전 증가 변수 (10도씩 회전)
	pnum = (360 / IncAngle) * dot;       // 점의 개수
	mpoint = (Point*)malloc(sizeof(Point)*pnum); // 점의 개수 할당
	
	mpoint[n].x = 0;   mpoint[n].y = -100;   mpoint[n++].z = 0;    
	mpoint[n].x = 0;   mpoint[n].y = 0;   mpoint[n++].z = 30;
	mpoint[n].x = 0;   mpoint[n].y = 0;   mpoint[n++].z = 24;//콘끝
	mpoint[n].x = 0;   mpoint[n].y = 5;   mpoint[n++].z = 26;
	mpoint[n].x = 0;   mpoint[n].y = 20;   mpoint[n++].z = 30;
	mpoint[n].x = 0;   mpoint[n].y = 30;   mpoint[n++].z = 26;
	mpoint[n].x = 0;   mpoint[n].y = 40;   mpoint[n++].z = 20;
	mpoint[n].x = 0;   mpoint[n].y = 45;   mpoint[n++].z = 20;//1단 아스크림끗
	mpoint[n].x = 0;   mpoint[n].y = 52;   mpoint[n++].z = 25;
	mpoint[n].x = 0;   mpoint[n].y = 58;   mpoint[n++].z = 26;
	mpoint[n].x = 0;   mpoint[n].y = 60;   mpoint[n++].z = 26;
	mpoint[n].x = 0;   mpoint[n].y = 70;   mpoint[n++].z = 23;
	mpoint[n].x = 0;   mpoint[n].y = 80;   mpoint[n++].z = 16;
	mpoint[n].x = 0;   mpoint[n].y = 83;   mpoint[n++].z = 10;
	mpoint[n].x = 0;   mpoint[n].y = 85;   mpoint[n++].z = 0;

	
	
	for (i = 0; i<pnum - dot; i++) {      // 회전에 의한 점 위치 계산
		mpoint[i + dot].x = cos(IncAngle*PI / 180)*mpoint[i].x - sin(IncAngle*PI / 180)*mpoint[i].z;
		mpoint[i + dot].y = mpoint[i].y;
		mpoint[i + dot].z = sin(IncAngle*PI / 180)*mpoint[i].x + cos(IncAngle*PI / 180)*mpoint[i].z;
	}

	fnum = dot *2* 360 / IncAngle;      // 면의 개수
	mface = (Face*)malloc(sizeof(Face)*fnum);      // 면의 개수 할당
	for (i = 0; i<fnum; i += 2) {      // 계산된 점들로 면 구성
		if ((j + 1) % dot != 0) {
			mface[i].ip[0] = j % pnum;
			mface[i].ip[1] = (j + 1) % pnum;
			mface[i].ip[2] = (j + 1 + dot) % pnum;
			mface[i + 1].ip[0] = j % pnum;
			mface[i + 1].ip[1] = (j + 1 + dot) % pnum;
			mface[i + 1].ip[2] = (j + dot) % pnum;
		}
		else i -= 2;
		j++;
	}
}

void MakeSweep()
{
	int i, j = 0;
	int IncAngle = 5;
	int dot = 4;
	int n = 0;
	pnum = (360 / IncAngle) * 12;
	mpoint = (Point*)malloc(sizeof(Point)*pnum);

	mpoint[n].x = 50;   mpoint[n].y = -30;   mpoint[n++].z = 50;
	mpoint[n].x = 0;   mpoint[n].y = -100;   mpoint[n++].z = 0;
	mpoint[n].x = 60;   mpoint[n].y = -100;   mpoint[n++].z = 60;
	mpoint[n].x = 50;   mpoint[n].y = -30;   mpoint[n++].z = 50;

	for (i = 0; i<pnum - 2; i++) {
		mpoint[i + dot].x = cos(IncAngle*PI / 180)*mpoint[i].x - sin(IncAngle*PI / 180)*mpoint[i].z - 3;
		mpoint[i + dot].y = mpoint[i].y + 11;
		mpoint[i + dot].z = sin(IncAngle*PI / 180)*mpoint[i].x + cos(IncAngle*PI / 180)*mpoint[i].z + 4;
	}

	fnum = 2 * 360 / IncAngle;
	mface = (Face*)malloc(sizeof(Face)*fnum);
	for (i = 0; i<fnum; i += 2)
	{
		mface[i].ip[0] = i % pnum;
		mface[i].ip[1] = (i + 1) % pnum;
		mface[i].ip[2] = (i + dot+1) % pnum;

		mface[i + 1].ip[0] = i % pnum;
		mface[i + 1].ip[1] = (i + dot+1) % pnum;
		mface[i + 1].ip[2] = (i + dot) % pnum;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	printf("key %d\n", key);
	switch (key)
	{
	case 'w':
		status = WIRE;
		glutPostRedisplay();
		break;
	case 's':
		status = SHADE;
		glutPostRedisplay();
		break;
	case 'c':
		if (cull) cull = 0;
		else cull = 1;
		glutPostRedisplay();
		break;
	case '1':
		fname = "cube.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '2':
		fname = "sphere.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '3':
		fname = "teapot.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '4':
		fname = "plane.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '5':
		MakeSORModel();
		glutPostRedisplay();
		break;
	case '6':
		MakeSweep();
		glutPostRedisplay();
		break;
	}
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Simple Modeling");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	ReadModel();
	GLSetupRC();
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}
