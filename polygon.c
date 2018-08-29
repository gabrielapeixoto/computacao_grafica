#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "GL/glut.h" 

#define MAXVERTEXS 30
#define NPOLYGON 6
#define PHI 3.141572

GLenum doubleBuffer;   
 
struct polygon 
{
    float v[3];
} pvertex[MAXVERTEXS];

int windW, windH;
int cc;
int tipoPoligono;
int ss = -1;
int numPontos = 0;

void circulo(float r, float ang, float pp[3])
{
	pp[0] = (float)(r * cos(ang));
	pp[1] = (float)(r * sin(ang));
	pp[2] = (float)0.0;
}


int clipVertex(int x, int y)
{
	int i;
	float d;
	ss=-1;   printf("\n clipVertex (%d, %d) ", x, y); 
	for (i=0; i<NPOLYGON; i++) {
		d = sqrt(pow((pvertex[i].v[0]-x), 2.0) + pow((pvertex[i].v[1]-y), 2.0));
		printf(" i: %i -> d:%4.2f;  ", i, d);
		if(d < 3.0){
			ss = i;
			break;
		}
	}
	return ss;
}

void init(void)
{
	int i;
	float ang;
	cc=1;
	tipoPoligono = GL_LINE;

	for(i=0; i<NPOLYGON; i++)
	{
		ang = (float)(i) * (2.0 * PHI)/((float)(NPOLYGON));
		circulo(70.0, ang, pvertex[i].v);
	}
}

static void Reshape(int width, int height)
{
    windW = width/2; 
    windH = height/2;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluPerspective(60.0, 1.0, 0.1, 1000.0);
    gluOrtho2D(-windW, windW, -windH, windH);

    glMatrixMode(GL_MODELVIEW);
}

static void Key(unsigned char key, int x, int y)
{
    switch (key) 
	{
      case 27:
			exit(0);
    }
}

void coord_line(void)
{
    glLineWidth(1);

	glColor3f(1.0, 0.0, 0.0);

	// vertical line

	glBegin(GL_LINE_STRIP);
		glVertex2f(-windW, 0);
		glVertex2f(windW, 0);
    glEnd();

	// horizontal line 

    glBegin(GL_LINE_STRIP);
		glVertex2f(0, -windH);
		glVertex2f(0, windH);
    glEnd();
}

void PolygonDraw(void)
{
	int i;

	glColor3f(0.0, 0.0, 0.0); 

	glPolygonMode(GL_FRONT_AND_BACK, tipoPoligono);

	glBegin(GL_POLYGON);
	for(i=0; i<NPOLYGON; i+=cc)
	{
		glVertex2fv(pvertex[i].v);
	}
	glEnd();
}

void PointDraw (void)
{ 
    int i;
    glColor3f(0.0, 0.0, 1.0);
	glPointSize(3);
	glBegin(GL_POINTS); 				
	for(i=0; i<NPOLYGON; i+=cc)
	{
		glVertex2fv(pvertex[i].v);
	} 
	glEnd();  
    
    if (ss>=0)
    {
        glColor3f(1.0, 0.0, 0.0);
	    glBegin(GL_POINTS); 
           glVertex2fv(pvertex[ss].v);
        glEnd();        
    }        
}

static void Draw(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

	coord_line();

	PolygonDraw();
	
	PointDraw();

    if (doubleBuffer) 
	{
	   glutSwapBuffers(); 
    } else {
	   glFlush();     
    }
}

void processMenuEvents(int option) 
{
	switch (option) 
	{
		case 4 : 
			if (tipoPoligono == GL_LINE)
				tipoPoligono = GL_FILL;
			else 
				tipoPoligono = GL_LINE;
			break;
	}
	glutPostRedisplay();
}

void processSubMenuEvents(int option) 
{
	cc = option;
	glutPostRedisplay();
}


static void Args(int argc, char **argv)
{
    GLint i;

    doubleBuffer = GL_FALSE;

    for (i = 1; i < argc; i++) 
	{
	   if (strcmp(argv[i], "-sb") == 0)   
	   {
	      doubleBuffer = GL_FALSE;
	   } else if (strcmp(argv[i], "-db") == 0) 
	   {
	      doubleBuffer = GL_TRUE;
	   }
    }
}

void createGLUTMenus() 
{
	int menu,submenu;

	submenu = glutCreateMenu(processSubMenuEvents);
	glutAddMenuEntry("Polígono",1);
	glutAddMenuEntry("Triângulo",2);
	glutAddMenuEntry("Retângulo",3);

	menu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Linha/Prenchido",4);
	glutAddSubMenu("Tipo Poligono",submenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void motionMouse(int x, int y)
{
	glColor3f(0.0, 1.0, 0.0);
	glPointSize(3);
	glBegin(GL_POINTS); 				
		x = x - windW;     y = windH - y;
		glVertex2i(x, y);  
	glEnd(); 
}

void mouseClick(int button, int state, int x, int y)
{ 
	if(button == GLUT_LEFT_BUTTON)
		if(state == GLUT_DOWN)
		{
			x = x - windW;    y = windH - y;
			ss = clipVertex(x, y);
			if(ss>-1) {
				glColor3f(1.0, 0.0, 0.0);
				glPointSize(3);
				glBegin(GL_POINTS); 
					glVertex2i(x, y);
				glEnd();
			}
		}
}

int main(int argc, char **argv)
{
    GLenum type;

    glutInit(&argc, argv);
    Args(argc, argv);

    type = GLUT_RGB;
    type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;

    glutInitDisplayMode(type);
    glutInitWindowSize(600, 500);
    glutCreateWindow("Basic Program Using Glut and Gl");

	init();

    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);

	glutMotionFunc(motionMouse); 
	glutMouseFunc(mouseClick);
//	glutIdleFunc(idle);

	createGLUTMenus();  

    glutMainLoop();

	return (0);
}