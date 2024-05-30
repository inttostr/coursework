#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;
bool textureReplace = true;		// (�) ��� ���������� �������� �� ����� �������

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'E')											// (�) ����� �������� ���������� �� ���� ������
	{
		textureReplace = !textureReplace;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;
GLuint texId2;									// (�) � �� ��� �� ������� ������, ������� � ������ ���������� ����� ����, �� ���� ���������� � ����� ������� "2", � �� ��������

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)																						(�)	��� ������ � ����������� ������
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);					// (�) ���� �� �������� � ����������, � ��������� ������ � ������� �� ���
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);


	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);																// (�) � ������


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray2;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray2;
	int texW2, texH2;
	OpenGL::LoadBMP("texture1.bmp", &texW2, &texH2, &texarray2);
	OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);


	//���������� �� ��� ��������
	glGenTextures(1, &texId2);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId2);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);


	//�������� ������
	free(texCharArray2);
	free(texarray2);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


//double EG[] = { -1, 11, 0 }; //polukrug dlya 40
//double FG[] = { -7, 6, 0 }; //polukrug dlya 40
//void drawCylinder(float r, float g, float b) {
//	glColor3d(0, 1, 0);
//
//	double i, resolution = 0.1;
//	double height = 2, radius = sqrt(61) / 2;
//	glPushMatrix();
//	//glTranslatef((EG[0] + FG[0]) / 2 - 0.1, (EG[1] + FG[1]) / 2, 1); //�������� �� ����
//	
//	glTranslatef((EG[0] + FG[0]) / 2, (EG[1] + FG[1]) / 2, 0); //�������� �� ����
//	glRotatef(38, 0, 0, 1);
//
//
//
//
//	/* bottom triangle: note: for is in reverse order */
//	glBegin(GL_TRIANGLE_FAN);
//	glVertex3f(0, 0, 0);  /* center */
//	for (i = 1 * PI + 0.06; i >= 0; i -= resolution)
//		glVertex3f(radius * cos(i), radius * sin(i), 0);
//	/* close the loop back to 0 degrees */
//	glVertex3f(radius * cos(0), radius * sin(0), 0);
//	glEnd();
//
//	/* middle tube */
//	glBegin(GL_QUAD_STRIP);
//	for (i = 0; i <= 1 * PI + 0.1; i += resolution)
//	{
//		glVertex3f(radius * cos(i), radius * sin(i), 0);
//		glVertex3f(radius * cos(i), radius * sin(i), height);
//	}
//	/* close the loop back to zero degrees */
//	glVertex3f(radius, 0, 0);
//	glVertex3f(radius * cos(0), radius * sin(0), 0);
//	glEnd();
//	
//	/* top triangle */
//	glEnable(GL_BLEND);											// (�) ����� �������� ������. ���� ����� ��������� ������ ��������, �� ������, ������� ������ ���� ����������(��� ����� ����� � ����������)
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glColor4d(1, 0, 0, 0.2);
//	glNormal3d(0, 0, 1);
//	glBegin(GL_TRIANGLE_FAN);
//
//	glVertex3f(0, 0, 2);  /* center */
//	for (i = 1 * PI + 0.06; i >= 0; i -= resolution)
//		glVertex3f(radius * cos(i), radius * sin(i), 2);
//	/* close the loop back to 0 degrees */
//	glVertex3f(radius * cos(0), radius * sin(0), 2);
//	glEnd();
//	
//	glPopMatrix();
//}

void draw_cylinder(GLfloat radius, GLfloat height, GLubyte R, GLubyte G, GLubyte B, GLfloat x1, GLfloat y1) {
	GLfloat x = 0;
	GLfloat y = 0;
	GLfloat angle = 0.0;
	GLfloat angle_stepsize = 0.1;

	/** Draw the tube */
	glColor3ub(R - 40, G - 40, B - 40);
	glBegin(GL_QUAD_STRIP);
	angle = 0.0;
	double tx = radius * cos(angle), ty = radius * sin(angle);
	while (angle < 3.14 + 0.06) {
		x = radius * cos(angle);
		y = radius * sin(angle);

		//���������� ������� ����� � ���� ���������
		double nx = -x * 0 + x * 3;
		double ny = -y * 0 + y * 3;
		double nz = -x * y + y * x;
		glNormal3d(nx, ny, nz);

		//glTexCoord2d(tx, 0);
		//glVertex3d(x, y, 0);
		//glTexCoord2d(tx, 0);
		//glVertex3d(x, 0, 0);
		//glTexCoord2d(ty, 1);
		//glVertex3d(x, y, 3);
		//glTexCoord2d(ty, 1);
		//glVertex3d(0, y, 3);
		//glEnd();

		glTexCoord2d(x, y);
		glVertex3d(x, y, 2);   //�������� ��� ������� ����� ������, �� ���� ������ ������ ���� �������� ��������� � ��
		glTexCoord2d(0, 0);
		glVertex3d(x, y, 0.0);

		angle = angle + angle_stepsize;
	}
	glEnd();

	/** Draw the circle on top of cylinder */
	glColor3ub(R, G, B);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 0);//������� � ���� ���������
	angle = 0.0;
	while (angle < 3.14 + 0.06) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glTexCoord2d(x, y);
		glVertex3f(x + x1, y + y1, 0);
		angle = angle + angle_stepsize;
	}
	//glVertex3f(radius + x1, y1, 0);
	glEnd();

	/** Draw the circle on top of cylinder */

	glColor3ub(R, G, B);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 0);//������� � ����� ���������
	angle = 0.0;
	while (angle < 3.14 + 0.06) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glTexCoord2d(x, y);
		glVertex3f(x + x1, y + y1, height);
		angle = angle + angle_stepsize;
	}
	glDisable(GL_BLEND);
	//glVertex3f(radius + x1, y1, height);
	glEnd();
}


void Render(OpenGL *ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	if (textureReplace)														// (�) ����� ������������� ��������: textureReplace - �������� �� 20 �������
		glBindTexture(GL_TEXTURE_2D, texId);								// (�) ���������� ������ �� 225
	else 
		glBindTexture(GL_TEXTURE_2D, texId2);

	
	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glDisable(GL_BLEND);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���						(�) ��� �����, �������� ������� �� ������ ���


	//������ ��������� ���������� ��������

	double A[] = { 0, 0, 0 };
	double B[] = { 4, -3, 0 };
	double C[] = { 2, 2, 0 };
	double D[] = { 6, 6, 0 };
	double E[] = { -1, 11, 0 }; //polukrug dlya 40
	double F[] = { -7, 6, 0 }; //polukrug dlya 40
	double G[] = { -3, 1, 0 };
	double H[] = { -5, -5, 0 };

	glColor3d(0.3, 0.2, 0.3);

	//������� ����� � ����
	double n0[] = { 0, 0 , 1 };

	glBegin(GL_POLYGON);
	glColor3d(1, 0, 0);
	glNormal3d(n0[0], n0[1], n0[2]);
	glTexCoord2d(0, 0); 
	glVertex3dv(A);
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	glTexCoord2d(1, 1); 
	glVertex3dv(C);
	glTexCoord2d(0, 1); 
	glVertex3dv(D);
	glTexCoord2d(0, 0); 
	glVertex3dv(E);
	glTexCoord2d(1, 0); 
	glVertex3dv(F);
	glTexCoord2d(1, 1); 
	glVertex3dv(G);
	glTexCoord2d(0, 1); 
	glVertex3dv(H);
	/*glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(H);*/
	glEnd();


	double A1[] = { 0, 0, 2 };
	double B1[] = { 4, -3, 2 };
	double C1[] = { 2, 2, 2 };
	double D1[] = { 6, 6, 2 };
	double E1[] = { -1, 11, 2 };
	double F1[] = { -7, 6, 2 };
	double G1[] = { -3, 1, 2 };
	double H1[] = { -5, -5, 2 };

	//glColor3d(0.7, 0.7, 0.7);
	//������� ����� � �����, ��� � ������
	double n1[] = { 0, 0 ,1 };

	//��� ������� ������
	double ABxAA1[] = { 3, -24, 0 };//��� �� ������� ������� � ������� ���������, � ���������� � ���� ��������� � �������� ��� ��� ������
	

	//glBegin(GL_TRIANGLES);
	////glNormal3d(0, 0, -1);					// (�) ��� �������
	//glColor3f(0.5f, 0.5f, 0.5f);
	//glVertex3dv(A);
	//glVertex3dv(B);
	//glVertex3dv(C);
	//glEnd();

	//glBegin(GL_TRIANGLES);
	////glNormal3d(0, 0, -1);
	//glColor3f(0.5f, 0.5f, 0.5f);
	//glVertex3dv(C);
	//glVertex3dv(D);
	//glVertex3dv(E);
	//glEnd();

	//glBegin(GL_TRIANGLES);
	////glNormal3d(0, 0, -1);
	//glColor3f(0.5f, 0.5f, 0.5f);
	//glVertex3dv(E);
	//glVertex3dv(F);
	//glVertex3dv(G);
	//glEnd();

	//glBegin(GL_TRIANGLES);
	////glNormal3d(0, 0, -1);
	//glColor3f(0.5f, 0.5f, 0.5f);
	//glVertex3dv(G);
	//glVertex3dv(E);
	//glVertex3dv(C);
	//glEnd();

	//glBegin(GL_TRIANGLES);
	////glNormal3d(0, 0, -1);
	//glColor3f(0.5f, 0.5f, 0.5f);
	//glVertex3dv(G);
	//glVertex3dv(H);
	//glVertex3dv(A);
	//glEnd();

	//glBegin(GL_TRIANGLES);
	////glNormal3d(0, 0, -1);
	//glColor3f(0.5f, 0.5f, 0.5f);
	//glVertex3dv(G);
	//glVertex3dv(C);
	//glVertex3dv(A);
	//glEnd();

	//drawCylinder(2, 2, 2);

	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.3f, 0.3f);
	glNormal3d(ABxAA1[0], ABxAA1[1], ABxAA1[2]);//��� �� ��������� ��� �������
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 1);
	glVertex3dv(A1);
	glTexCoord2d(1, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	glEnd();

	double BCxBB1[] = { 12, 6, 0 };
	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.3f, 0.3f);
	glNormal3d(BCxBB1[0], BCxBB1[1], BCxBB1[2]);
	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(1, 0);
	glVertex3dv(C);
	glEnd();

	double CDxCC1[] = { -3, 18, 0 };
	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.3f, 0.3f);
	glNormal3d(CDxCC1[0], CDxCC1[1], CDxCC1[2]);
	glTexCoord2d(0, 0);							// (�) ��� ������������� �������� (x, y) ��� ���� ��������, ������� ������ ����� ���������
	glVertex3dv(C);
	glTexCoord2d(0, 1);
	glVertex3dv(C1);
	glTexCoord2d(1, 1);
	glVertex3dv(D1);
	glTexCoord2d(1, 0);
	glVertex3dv(D);
	glEnd();


	double DExDD1[] = { 15, 9, 0 };
	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.3f, 0.3f);
	glNormal3d(DExDD1[0], DExDD1[1], DExDD1[2]);
	glTexCoord2d(0, 0);
	glVertex3dv(D);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glTexCoord2d(1, 1);
	glVertex3dv(E1);
	glTexCoord2d(1, 0);
	glVertex3dv(E);
	glEnd();

	//glBegin(GL_QUADS);
	//glColor3f(0.3f, 0.3f, 0.3f);
	////glNormal3d(0.6, 0.8, 0);
	//glTexCoord2d(0, 0);
	//glVertex3dv(E);
	//glTexCoord2d(1, 0);
	//glVertex3dv(E1);
	//glTexCoord2d(1, 1);
	//glVertex3dv(F1);
	//glTexCoord2d(0, 1);
	//glVertex3dv(F);
	//glEnd();


	double FGxFF1[] = { -27, 6, 0 };
	glBegin(GL_QUADS);
	glNormal3d(FGxFF1[0], FGxFF1[1], FGxFF1[2]);
	glTexCoord2d(0, 0);
	glVertex3dv(F);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	glTexCoord2d(1, 1);
	glVertex3dv(G1);
	glTexCoord2d(1, 0);
	glVertex3dv(G);
	glEnd();


	double GHxGG1[] = { 3, -18, 0 };
	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.3f, 0.3f);
	glNormal3d(GHxGG1[0], GHxGG1[1], GHxGG1[2]);
	glTexCoord2d(0, 0);
	glVertex3dv(G);
	glTexCoord2d(0, 1);
	glVertex3dv(G1);
	glTexCoord2d(1, 1);
	glVertex3dv(H1);
	glTexCoord2d(1, 0);
	glVertex3dv(H);
	glEnd();


	double HAxHH1[] = { 12, -3, 0 };
	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.3f, 0.3f);
	glNormal3d(HAxHH1[0], HAxHH1[1], HAxHH1[2]);
	glTexCoord2d(0, 0);
	glVertex3dv(H);
	glTexCoord2d(0, 1);
	glVertex3dv(H1);
	glTexCoord2d(1, 1);
	glVertex3dv(A1);
	glTexCoord2d(1, 0);
	glVertex3dv(A);
	glEnd();



	glColor3d(0.4, 0, 0.3);

	glPushMatrix();
	double EG[] = { -1, 11, 0 }; //polukrug dlya 40
	double FG[] = { -7, 6, 0 }; //polukrug dlya 40
	double i, resolution = 0.1;
	double height = 2, radius = sqrt(61) / 2;
	glTranslatef((EG[0] + FG[0]) / 2, (EG[1] + FG[1]) / 2, 0);
	//glTranslated(-2.4, 9, 0);
	glRotatef(38, 0, 0, 1);
	//draw_cylinder(3.87, 3, 255, 160, 100, 0, 0);
	//drawCylinder(2, 2, 2);
	draw_cylinder(3.9, 2, 255, 160, 100, 0, 0);
	glPopMatrix();


	glColor4d(1, 1, 1, 0.5);
	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glBegin(GL_POLYGON);
	glNormal3d(n1[0], n1[1], n1[2]);
	glTexCoord2d(0, 0);
	glVertex3dv(A1);
	glTexCoord2d(1, 0);
	glVertex3dv(B1);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glTexCoord2d(0, 0);
	glVertex3dv(E1);
	glTexCoord2d(1, 0);
	glVertex3dv(F1);
	glTexCoord2d(1, 1);
	glVertex3dv(G1);
	glTexCoord2d(0, 1);
	glVertex3dv(H1);
	glEnd();
	glDisable(GL_BLEND);

	

   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 200);
	rec.setPosition(10, ogl->getHeight() - 200 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "E - ������������ �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	ss << "UV-��������" << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}