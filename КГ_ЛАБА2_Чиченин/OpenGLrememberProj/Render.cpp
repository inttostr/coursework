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
bool textureReplace = true;		// (Н) Эта переменная отвечает за смену текстур

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
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
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
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
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
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

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
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

	if (key == 'E')											// (Н) Смена текстуры происходит по этой кнопке
	{
		textureReplace = !textureReplace;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;
GLuint texId2;									// (Н) Я хз как по другому делать, поэтому я просто скопировал часть кода, ко всем переменным в конце добавил "2", и всё работает

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)																						(Н)	Код брался и копировался отсюда
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);					// (Н) Чтоб не мучаться с картинками, я коприовал старую и рисовал на ней
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);


	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);																// (Н) И досюда


	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE* texarray2;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray2;
	int texW2, texH2;
	OpenGL::LoadBMP("texture1.bmp", &texW2, &texH2, &texarray2);
	OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);


	//генерируем ИД для текстуры
	glGenTextures(1, &texId2);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId2);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);


	//отчистка памяти
	free(texCharArray2);
	free(texarray2);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

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
//	//glTranslatef((EG[0] + FG[0]) / 2 - 0.1, (EG[1] + FG[1]) / 2, 1); //смещение по осям
//	
//	glTranslatef((EG[0] + FG[0]) / 2, (EG[1] + FG[1]) / 2, 0); //смещение по осям
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
//	glEnable(GL_BLEND);											// (Н) Здесь строится крышка. Чтоб Альфа наложение хорошо работало, то объект, который должен быть прозрачным(для этого альфа и существует)
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

		//добавление нормали света к боку циллиндру
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
		glVertex3d(x, y, 2);   //значения для нормали берем отсюда, по идее можешь просто свои значения поставить и всё
		glTexCoord2d(0, 0);
		glVertex3d(x, y, 0.0);

		angle = angle + angle_stepsize;
	}
	glEnd();

	/** Draw the circle on top of cylinder */
	glColor3ub(R, G, B);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 0);//нормаль к низу циллиндра
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
	glNormal3d(0, 0, 0);//нормаль к верху циллиндра
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

	if (textureReplace)														// (Н) Здесь переключаются текстуры: textureReplace - создаётся на 20 строчке
		glBindTexture(GL_TEXTURE_2D, texId);								// (Н) Назначение кнопки на 225
	else 
		glBindTexture(GL_TEXTURE_2D, texId2);

	
	//альфаналожение
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glDisable(GL_BLEND);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут						(Н) это обман, пришлось прогать не только тут


	//Начало рисования квадратика станкина

	double A[] = { 0, 0, 0 };
	double B[] = { 4, -3, 0 };
	double C[] = { 2, 2, 0 };
	double D[] = { 6, 6, 0 };
	double E[] = { -1, 11, 0 }; //polukrug dlya 40
	double F[] = { -7, 6, 0 }; //polukrug dlya 40
	double G[] = { -3, 1, 0 };
	double H[] = { -5, -5, 0 };

	glColor3d(0.3, 0.2, 0.3);

	//нормаль света к низу
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
	//нормаль света к верху, так и оставь
	double n1[] = { 0, 0 ,1 };

	//для боковых граней
	double ABxAA1[] = { 3, -24, 0 };//это мы считаем нормаль к боковой плоскости, в материалах к лабе расписано с рисунком как это делать
	

	//glBegin(GL_TRIANGLES);
	////glNormal3d(0, 0, -1);					// (Н) Это нормаль
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
	glNormal3d(ABxAA1[0], ABxAA1[1], ABxAA1[2]);//тут мы применяем эту нормаль
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
	glTexCoord2d(0, 0);							// (Н) Это накладывается текстура (x, y) это углы текстуры, пишутся поверх углов квадратов
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

	

   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 200);
	rec.setPosition(10, ogl->getHeight() - 200 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "E - Переключение текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	ss << "UV-развёртка" << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}