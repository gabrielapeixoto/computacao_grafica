#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "GL/glut.h" 

#define MAXVERTEXS 30
#define NVERTICE 6
#define PHI 3.141572

#define POLIGONO   0
#define VERTICE    1
#define TRANSLACAO 2
#define ROTACAO    3
#define ESCALA     4
#define CISALHA    5

GLenum doubleBuffer;   
 
typedef struct polygon 
{
    float v[3];
} typePolygon;

typePolygon pvertex[MAXVERTEXS];

int windW, windH;       // horizontal e vertical da janela
int gOpcao;                 // operacao escolhido no menu
int tipoPoligono;
int gIndVert = -1;      // indicador do vértice manipulado
int numPontos = 0;
int gXant, gYant;		// posicao (x, y) anterior

float **MM;		// matriz generico

float Mt[3][3] = {{1.0, 0.0, 0.0},
			{0.0, 1.0, 0.0},
			{0.0, 0.0, 1.0}};
float Mr[3][3] = {{1.0, 0.0, 0.0},
			{0.0, 1.0, 0.0},
			{0.0, 0.0, 1.0}};
float Ms[3][3] = {{1.0, 0.0, 0.0},
			{0.0, 1.0, 0.0},
			{0.0, 0.0, 1.0}};
			
void circulo(float r, float ang, float pp[3])
{
	pp[0] = (float)(r * cos(ang));
	pp[1] = (float)(r * sin(ang));
	pp[2] = (float)1.0;
}


int clipVertex(int x, int y)
{
	int i;
	float d;
	gIndVert=-1;   printf("\n clipVertex (%d, %d) ", x, y); 
	for (i=0; i<NVERTICE; i++) {
		d = sqrt(pow((pvertex[i].v[0]-x), 2.0) + pow((pvertex[i].v[1]-y), 2.0));
		if(d < 3.0){
			gIndVert = i;     // o indice foi selecionado
			printf(" vertice i: %i -> d:%4.2f;  ", i, d);
			break;
		}
	}
	return gIndVert;
}

void init(void)
{
	int i;
	float ang, vAng;
	gOpcao=POLIGONO;
	tipoPoligono = GL_LINE;

	vAng = 2.0 * PHI/((float)(NVERTICE));
	for(i=0; i<NVERTICE; i++)
	{
		ang = (float)(i) * vAng;
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
	for(i=0; i<NVERTICE; i++)     // estava como i+=cc ?
	{
		glVertex2fv(pvertex[i].v);
	}
	glEnd();
}

void PointDraw (void)
{  
    int i;

	glPointSize(4);
	
    glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POINTS); 				
	for(i=0; i<NVERTICE; i++)     
		glVertex2fv(pvertex[i].v);
	glEnd();  
    
    if (gIndVert>=0)	// vertice gIndVert selecionado?
    {
        glColor3f(1.0, 0.0, 0.0);     // cor vermelho do vertice selecionado
	    glBegin(GL_POINTS); 
           glVertex2fv(pvertex[gIndVert].v);
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
	gOpcao = 0;
	if(option>0 && option<6)
		gOpcao = option;
	printf("\n opcao %d ", gOpcao);
	glutPostRedisplay();
}

void processSubMenuEvents(int option) 
{
	gOpcao = option;
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

	submenu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Translacao", TRANSLACAO);
	glutAddMenuEntry("Rotacao", ROTACAO);
	glutAddMenuEntry("Escalar", ESCALA);
	glutAddMenuEntry("Cisalha", CISALHA);

	menu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Mover Vertice", VERTICE);
	glutAddSubMenu("Transformacao",submenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

float anguloRotacao(int vx, int vy) {
	// arccos ((va.vb)/(||va|| ||vb||)))	
	float prodNorma, prodVetor, ang;
	printf("\n xy ant (%d, %d)", gXant, gYant);
	printf("--> xy atual (%d, %d)", vx, vy);	
	prodNorma = sqrt((double)(gXant*gXant + gYant*gYant));
	prodNorma *= sqrt((double)(vx*vx + vy*vy));
	
	prodVetor = (float)vx*gXant + (float)vy*gYant;
	
	printf("\n numera: %6.4, denum: %6.4 ", prodVetor, prodNorma);
	
	ang = acos(prodVetor/prodNorma);
	
	return (ang);
}

void operaVertices(int n, typePolygon pv[] ) {
	int k;
	float x, y, z;
	for(k=0; k<n; k++) {  // para cada vertice
		x = Mt[0][0]*pv[k].v[0] + Mt[0][1]*pv[k].v[1] + Mt[0][2]*pv[k].v[2];
		y = Mt[1][0]*pv[k].v[0] + Mt[1][1]*pv[k].v[1] + Mt[1][2]*pv[k].v[2];
		z = Mt[2][0]*pv[k].v[0] + Mt[2][1]*pv[k].v[1] + Mt[2][2]*pv[k].v[2];
		pv[k].v[0] = x;
		pv[k].v[1] = y;
		pv[k].v[2] = z;
	}
}

void transRota(float ang) {
	int indV;
	float x, y;
	Mr[0][0] = Mr[1][1] = cos(ang);
	Mr[1][1] = sin(ang);
	Mr[0][1] = -1.0 * Mr[1][1];
	//MM = Mr;
	operaVertices(NVERTICE, pvertex);
}

void transTraslada(int dx, int dy) {
	int indV;
	float x, y;
	Mt[0][2] = dx;
	Mt[1][2] = dy;
	//MM = Mt;
    operaVertices(NVERTICE, pvertex);
}

void motionMouse(int x, int y)
{
	float ang;
	x = x - windW;     y = windH - y;
	printf("\n motionMouse: gOpcao %d", gOpcao);
	if(gOpcao == VERTICE) {
		if(gIndVert>-1)	{
			pvertex[gIndVert].v[0] = x;
			pvertex[gIndVert].v[1] = y;
		//	glutPostRedisplay();
		}
	} else {
		if(gOpcao == TRANSLACAO) {
			transTraslada(x - gXant, y - gYant);	
		} else {
			if(gOpcao == ROTACAO) {
				ang = anguloRotacao(x, y);
				transRota(ang); 
				printf (" -- Ang: %6.4", ang);
			}
		}
		
	}
	gXant = x;
	gYant = y;
	glutPostRedisplay();	
}

void mouseClick(int button, int state, int x, int y)
{ 
	if(button == GLUT_LEFT_BUTTON) {
		if(gOpcao>0 && gOpcao<6)
			if(state == GLUT_DOWN) {
				x = x - windW;    y = windH - y;
				gIndVert = clipVertex(x, y);
				if(gIndVert>-1) {
					gXant = x; 
					gYant = y;
				}
			}
			else {
				gIndVert = -1;
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
