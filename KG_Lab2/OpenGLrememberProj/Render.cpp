#include "Render.h"

#include <sstream>
#include <iostream>
#include <math.h>
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

double GetLenght(double x1, double y1, double x2, double y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

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
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

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
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


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





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ��� 
	void Render(double delta_time);
	{
		double A[] = { 2,0,0 };
		double B[] = { 0,7,0 };
		double C[] = { 7,9,0 };
		double D[] = { 8,15,0 };
		double E[] = { 15,11,0 };
		double F[] = { 11,7,0 };
		double G[] = { 14,1,0 };
		double H[] = { 8,5,0 };
		double N[] = { 1,3.5,0 };
		double M[] = { 11.5,13,0 };

		double A1[] = { 2,0,5 };
		double B1[] = { 0,7,5 };
		double C1[] = { 7,9,5 };
		double D1[] = { 8,15,5 };
		double E1[] = { 15,11,5 };
		double F1[] = { 11,7,5 };
		double G1[] = { 14,1,5 };
		double H1[] = { 8,5,5 };
		double N1[] = { 1,3.5,5 };
		double M1[] = { 11.5,13,5 };

		double R = (sqrt(4 + 49)) / 2;
		double angle = acos((B[0] - N[0]) / R);
		double Temp[] = { 0, 7, 0 };
		double Temp2[] = { 0, 7, 0 };
		
		double d, l, s;

		d = GetLenght(A[0], A[1], B[0], B[1]) * M_PI / 2;
		l = max(d, 5);

		double x1 = 0, x2 = 0;

		for (double i = 0.0; i <= M_PI + 0.1; i += 0.1)
		{
			double x = N[0] + R * (cos(angle) * cos(i) - sin(angle) * sin(i));
			double y = N[1] + R * (sin(angle) * cos(i) + cos(angle) * sin(i));
			Temp[0] = x;
			Temp[1] = y;
			if (i > M_PI)
			{

				Temp[0] = A[0];
				Temp[1] = A[1];
			}

			glBindTexture(GL_TEXTURE_2D, texId);
			glBegin(GL_TRIANGLES);
			glNormal3d(0, 0, -1);
			glColor3d(0.5, 0.5, 0.25);

			glVertex3dv(N);
			glVertex3dv(Temp);
			glVertex3dv(Temp2);
			glNormal3d(0, 0, 1);

			glTexCoord2d(0.5 + N[0] * 0.5 / 15, 0.5 + N[1] * 0.5 / 15 - 0.2);
			glVertex3d(N[0], N[1], N[2] + 5);
			glTexCoord2d(0.5 + Temp[0] * 0.5 / 15, 0.5 + Temp[1] * 0.5 / 15 - 0.2);
			glVertex3d(Temp[0], Temp[1], Temp[2] + 5);
			glTexCoord2d(0.5 + Temp2[0] * 0.5 / 15, 0.5 + Temp2[1] * 0.5 / 15 - 0.2);
			glVertex3d(Temp2[0], Temp2[1], Temp2[2] + 5);
			glEnd();

			s = GetLenght(Temp[0], Temp[1], Temp2[0], Temp2[1]);
			x1 += s/l;

			glBindTexture(GL_TEXTURE_2D, texId);
			glBegin(GL_QUADS);
			glNormal3d((Temp[1] - Temp2[1]), -(Temp[0] - Temp2[0]), 0);
			glColor3d(0.2, 0.7, 0.7);



			glTexCoord2d(x1, 0.5 - 5 / 2 / l);
			glVertex3dv(Temp);
			glTexCoord2d(x2, 0.5 - 5 / 2 / l);
			glVertex3dv(Temp2);
			glTexCoord2d(x2, 0.5 + 5 / 2 / l);
			glVertex3d(Temp2[0], Temp2[1], Temp2[2] + 5);
			glTexCoord2d(x1, 0.5 + 5 / 2 / l);
			glVertex3d(Temp[0], Temp[1], Temp[2] + 5);
			glEnd();

			x2 += s/l;
			Temp2[0] = Temp[0];
			Temp2[1] = Temp[1];
		}

		R = (sqrt(49 + 16)) / 2;
		angle = acos((D[0] - M[0]) / R);
		Temp[0] = 8;
		Temp[1] = 15;
		Temp2[0] = 8;
		Temp2[1] = 15;

		d = GetLenght(A[0], A[1], B[0], B[1]) * M_PI / 2;
		l = max(d, 5);
		x1 = 0;
		x2 = 0;
		for (double i = 0.0; i <= M_PI; i += 0.1)
		{
			double x = M[0] + R * (cos(angle) * cos(i) - sin(angle) * sin(i));
			double y = M[1] + R * (sin(angle) * cos(i) + cos(angle) * sin(i));
			Temp[0] = x;
			Temp[1] = y;
			x1 += s / l;
			if (i < M_PI / 2 && (((Temp[0] - D[0]) * (C[1] - D[1])) - (C[0] - D[0]) * (Temp[1] - D[1]) < 0))
			{
				glBindTexture(GL_TEXTURE_2D, texId);
				glBegin(GL_TRIANGLES);
				glNormal3d(0, 0, -1);
				glColor3d(0.5, 0.5, 0.25);

				glVertex3dv(C);
				glVertex3dv(Temp);
				glVertex3dv(Temp2);
				glNormal3d(0, 0, 1);

				glTexCoord2d(0.5 + C[0] * 0.5 / 15, 0.5 + C[1] * 0.5 / 15 - 0.2);
				glVertex3d(C[0], C[1], C[2] + 5);
				glTexCoord2d(0.5 + Temp[0] * 0.5 / 15, 0.5 + Temp[1] * 0.5 / 15 - 0.2);
				glVertex3d(Temp[0], Temp[1], Temp[2] + 5);
				glTexCoord2d(0.5 + Temp2[0] * 0.5 / 15, 0.5 + Temp2[1] * 0.5 / 15 - 0.2);
				glVertex3d(Temp2[0], Temp2[1], Temp2[2] + 5);
				glEnd();

				glBindTexture(GL_TEXTURE_2D, texId);
				glBegin(GL_QUADS);

				glTexCoord2d(x1, 0.5 - 5 / 2 / l);
				glVertex3dv(Temp);
				glTexCoord2d(x2, 0.5 - 5 / 2 / l);
				glVertex3dv(Temp2);
				glTexCoord2d(x2, 0.5 + 5 / 2 / l);
				glVertex3d(Temp2[0], Temp2[1], Temp2[2] + 5);
				glTexCoord2d(x1, 0.5 + 5 / 2 / l);
				glVertex3d(Temp[0], Temp[1], Temp[2] + 5);
				glEnd();
				Temp2[0] = Temp[0];
				Temp2[1] = Temp[1];
			}
			else if (i > M_PI / 2 && ((Temp[0] - F[0]) * (E[1] - F[1])) - (E[0] - F[0]) * (Temp[1] - F[1]) < 0)
			{
				glBindTexture(GL_TEXTURE_2D, texId);
				glBegin(GL_TRIANGLES);
				glNormal3d(0, 0, -1);
				glColor3d(0.5, 0.5, 0.25);

				glVertex3dv(F);
				glVertex3dv(Temp);
				glVertex3dv(Temp2);
				glNormal3d(0, 0, 1);

				glTexCoord2d(0.5 + F[0] * 0.5 / 15, 0.5 + F[1] * 0.5 / 15 - 0.2);
				glVertex3d(F[0], F[1], F[2] + 5);
				glTexCoord2d(0.5 + Temp[0] * 0.5 / 15, 0.5 + Temp[1] * 0.5 / 15 - 0.2);
				glVertex3d(Temp[0], Temp[1], Temp[2] + 5);
				glTexCoord2d(0.5 + Temp2[0] * 0.5 / 15, 0.5 + Temp2[1] * 0.5 / 15 - 0.2);
				glVertex3d(Temp2[0], Temp2[1], Temp2[2] + 5);
				glEnd();

				glBindTexture(GL_TEXTURE_2D, texId);
				glBegin(GL_QUADS);
				glNormal3d((Temp[1] - Temp2[1]), -(Temp[0] - Temp2[0]), 0);
				glColor3d(0.2, 0.7, 0.7);

				glTexCoord2d(x1, 0.5 - 5 / 2 / l);
				glVertex3dv(Temp);
				glTexCoord2d(x2, 0.5 - 5 / 2 / l);
				glVertex3dv(Temp2);
				glTexCoord2d(x2, 0.5 + 5 / 2 / l);
				glVertex3d(Temp2[0], Temp2[1], Temp2[2] + 5);
				glTexCoord2d(x1, 0.5 + 5 / 2 / l);
				glVertex3d(Temp[0], Temp[1], Temp[2] + 5);
				glEnd();
				Temp2[0] = Temp[0];
				Temp2[1] = Temp[1];
			}
			if (i <= M_PI / 2 + 0.1 && i >= M_PI / 2 - 0.1)
			{
				glBindTexture(GL_TEXTURE_2D, texId);
				glBegin(GL_TRIANGLES);
				glNormal3d(0, 0, -1);
				glColor3d(0.5, 0.5, 0.25);

				glVertex3dv(C);
				glVertex3dv(F);
				glVertex3dv(Temp);
				glNormal3d(0, 0, 1);

				glTexCoord2d(0.5 + C[0] * 0.5 / 15, 0.5 + C[1] * 0.5 / 15 - 0.2);
				glVertex3d(C[0], C[1], C[2] + 5);
				glTexCoord2d(0.5 + F[0] * 0.5 / 15, 0.5 + F[1] * 0.5 / 15 - 0.2);
				glVertex3d(F[0], F[1], F[2] + 5);
				glTexCoord2d(0.5 + Temp[0] * 0.5 / 15, 0.5 + Temp[1] * 0.5 / 15 - 0.2);
				glVertex3d(Temp[0], Temp[1], Temp[2] + 5);
				glEnd();

				glBindTexture(GL_TEXTURE_2D, texId);
				glBegin(GL_QUADS);
				glNormal3d((Temp[1] - Temp2[1]), -(Temp[0] - Temp2[0]), 0);
				glColor3d(0.2, 0.7, 0.7);

				glTexCoord2d(x1, 0.5 - 5 / 2 / l);
				glVertex3dv(Temp);
				glTexCoord2d(x2, 0.5 - 5 / 2 / l);
				glVertex3dv(Temp2);
				glTexCoord2d(x2, 0.5 + 5 / 2 / l);
				glVertex3d(Temp2[0], Temp2[1], Temp2[2] + 5);
				glTexCoord2d(x1, 0.5 + 5 / 2 / l);
				glVertex3d(Temp[0], Temp[1], Temp[2] + 5);
				glEnd();
				Temp2[0] = Temp[0];
				Temp2[1] = Temp[1];
			}
			x2 += s / l;
		}

		
		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_TRIANGLES);
		glNormal3d(0, 0, -1);
		glColor3d(0.5, 0.5, 0.25);

		glVertex3dv(A);
		glVertex3dv(B);
		glVertex3dv(H);

		glVertex3dv(C);
		glVertex3dv(B);
		glVertex3dv(H);

		glVertex3dv(C);
		glVertex3dv(F);
		glVertex3dv(H);

		glVertex3dv(G);
		glVertex3dv(F);
		glVertex3dv(H);

		//glVertex3dv(C);
		//glVertex3dv(F);
		//glVertex3dv(D);

		//glVertex3dv(E);
		//glVertex3dv(F);
		//glVertex3dv(D);

		glNormal3d(0, 0, 1);

		glTexCoord2d(0.5 + A[0] * 0.5 / 15, 0.5 + A[1] * 0.5 / 15 - 0.2);
		glVertex3dv(A1);
		glTexCoord2d(0.5 + B[0] * 0.5 / 15, 0.5 + B[1] * 0.5 / 15 - 0.2);
		glVertex3dv(B1);
		glTexCoord2d(0.5 + H[0] * 0.5 / 15, 0.5 + H[1] * 0.5 / 15 - 0.2);
		glVertex3dv(H1);

		glTexCoord2d(0.5 + C[0] * 0.5 / 15, 0.5 + C[1] * 0.5 / 15 - 0.2);
		glVertex3dv(C1);
		glTexCoord2d(0.5 + B[0] * 0.5 / 15, 0.5 + B[1] * 0.5 / 15 - 0.2);
		glVertex3dv(B1);
		glTexCoord2d(0.5 + H[0] * 0.5 / 15, 0.5 + H[1] * 0.5 / 15 - 0.2);
		glVertex3dv(H1);

		glTexCoord2d(0.5 + C[0] * 0.5 / 15, 0.5 + C[1] * 0.5 / 15 - 0.2);
		glVertex3dv(C1);
		glTexCoord2d(0.5 + F[0] * 0.5 / 15, 0.5 + F[1] * 0.5 / 15 - 0.2);
		glVertex3dv(F1);
		glTexCoord2d(0.5 + H[0] * 0.5 / 15, 0.5 + H[1] * 0.5 / 15 - 0.2);
		glVertex3dv(H1);

		glTexCoord2d(0.5 + G[0] * 0.5 / 15, 0.5 + G[1] * 0.5 / 15 - 0.2);
		glVertex3dv(G1);
		glTexCoord2d(0.5 + F[0] * 0.5 / 15, 0.5 + F[1] * 0.5 / 15 - 0.2);
		glVertex3dv(F1);
		glTexCoord2d(0.5 + H[0] * 0.5 / 15, 0.5 + H[1] * 0.5 / 15 - 0.2);
		glVertex3dv(H1);

		//glVertex3dv(C1);
		//glVertex3dv(F1);
		//glVertex3dv(D1);

		//glVertex3dv(E1);
		//glVertex3dv(F1);
		//glVertex3dv(D1);

		glEnd();


		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);

		glColor3d(0.2, 0.7, 0.7);

		//glVertex3dv(A1);
		//glVertex3dv(A);
		//glVertex3dv(B);
		//glVertex3dv(B1);

		d = GetLenght(B[0], B[1], C[0], C[1]);
		l = max(d, 5);

		glNormal3d((B[1] - C[1]), -(B[0] - C[0]), 0);
		glTexCoord2d(0.5 + d / 2 / l, 0 + 5 / 2 / l);
		glVertex3dv(B1);
		glTexCoord2d(0.5 + d / 2 / l, 0 - 5 / 2 / l);
		glVertex3dv(B);
		glTexCoord2d(0.5 - d / 2 / l, 0 - 5 / 2 / l);
		glVertex3dv(C);
		glTexCoord2d(0.5 - d / 2 / l, 0 + 5 / 2 / l);
		glVertex3dv(C1);

		d = GetLenght(D[0], D[1], C[0], C[1]);
		l = max(d, 5);

		glNormal3d((C[1] - D[1]), -(C[0] - D[0]), 0);
		glTexCoord2d(0.5 - d / 2 / l, 0 - 5 / 2 / l);
		glVertex3dv(C);
		glTexCoord2d(0.5 - d / 2 / l, 0 + 5 / 2 / l);
		glVertex3dv(C1);
		glTexCoord2d(0.5 + d / 2 / l, 0 + 5 / 2 / l);
		glVertex3dv(D1);
		glTexCoord2d(0.5 + d / 2 / l, 0 - 5 / 2 / l);
		glVertex3dv(D);

		//glVertex3dv(E);
		//glVertex3dv(E1);
		//glVertex3dv(D1);
		//glVertex3dv(D);

		d = GetLenght(E[0], E[1], F[0], F[1]);
		l = max(d, 5);

		glNormal3d((E[1] - F[1]), -(E[0] - F[0]), 0);
		glTexCoord2d(0.5 + d / 2 / l, 0.5 - 5 / 2 / l);
		glVertex3dv(E);
		glTexCoord2d(0.5 + d / 2 / l, 0.5 + 5 / 2 / l);
		glVertex3dv(E1);
		glTexCoord2d(0.5 - d / 2 / l, 0.5 + 5 / 2 / l);
		glVertex3dv(F1);
		glTexCoord2d(0.5 - d / 2 / l, 0.5 - 5 / 2 / l);
		glVertex3dv(F);

		d = GetLenght(F[0], F[1], G[0], G[1]);
		l = max(d, 5);

		glNormal3d((F[1] - G[1]), -(F[0] - G[0]), 0);
		glTexCoord2d(0.5 - d / 2 / l, 0.5 - 5 / 2 / l);
		glVertex3dv(F);
		glTexCoord2d(0.5 - d / 2 / l, 0.5 + 5 / 2 / l);
		glVertex3dv(F1);
		glTexCoord2d(0.5 + d / 2 / l, 0.5 + 5 / 2 / l);
		glVertex3dv(G1);
		glTexCoord2d(0.5 + d / 2 / l, 0.5 - 5 / 2 / l);
		glVertex3dv(G);

		d = GetLenght(G[0], G[1], H[0], H[1]);
		l = max(d, 5);

		glNormal3d((G[1] - H[1]), -(G[0] - H[0]), 0);
		glTexCoord2d(0.5 - d / 2 / l, 0.5 - 5 / 2 / l);
		glVertex3dv(H);
		glTexCoord2d(0.5 - d / 2 / l, 0.5 + 5 / 2 / l);
		glVertex3dv(H1);
		glTexCoord2d(0.5 + d / 2 / l, 0.5 + 5 / 2 / l);
		glVertex3dv(G1);
		glTexCoord2d(0.5 + d / 2 / l, 0.5 - 5 / 2 / l);
		glVertex3dv(G);

		d = GetLenght(H[0], H[1], A[0], A[1]);
		l = max(d, 5);

		glNormal3d((H[1] - A[1]), -(H[0] - A[0]), 0);
		glTexCoord2d(0.5 + d / 2 / l, 0.5 - 5 / 2 / l);
		glVertex3dv(H);
		glTexCoord2d(0.5 + d / 2 / l, 0.5 + 5 / 2 / l);
		glVertex3dv(H1);
		glTexCoord2d(0.5 - d / 2 / l, 0.5 + 5 / 2 / l);
		glVertex3dv(A1);
		glTexCoord2d(0.5 - d / 2 / l, 0.5 - 5 / 2 / l);
		glVertex3dv(A);

		glEnd();
	}
	//������ ��������� ���������� ��������
	/*double A[2] = {-4, -4};
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������


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
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}