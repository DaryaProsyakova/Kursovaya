#include "Render.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"
#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include "MyShaders.h"
#include "ObjLoader.h"
#include "GUItextRectangle.h"
#include "Texture.h"

#include "Fishes.h"

#define TOP_RIGHT 1.0f,1.0f
#define TOP_LEFT 0.0f,1.0f
#define BOTTOM_RIGHT 1.0f,0.0f
#define BOTTOM_LEFT 0.0f,0.0f


GLuint texId[3];
GuiTextRectangle rec;
bool textureMode = true;
bool lightMode = true;
bool shadow = false;
//��������� ������ ��� ��������� ����
#define POP glPopMatrix()
#define PUSH glPushMatrix()
ObjFile *model;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

Shader s[10];  //��������� ��� ������ ��������
Shader frac;
Shader cassini;

enum SelectFish //������������ ��� ������ �����
{
	Plotva1,
	Tuna1,
	Clown1,
	Dora1,
	Plotva2,
	Tuna2,
	Clown2,
	Dora2,
	Plotva3,
	Tuna3,
	Clown3,
	Dora3,
	YourFish // ����� ��� ���������� � ����������
};

enum SelectYourFish
{
	Plotv,
	Tun,
	Clow,
	Dor
};

SelectFish selector; // ��� ������, �� ����� ������ ���������

SelectYourFish selectorForYourFish; // ����� ��������, ������� ���������

Vector3 SelectedFishPos; //���������� ��������� �����

Vector3 YourPos; //������� ����������� �����
double Angle = 0; //���� ����������� �����

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	Vector3 cameraFront;

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
		SetLookPoint(SelectedFishPos);

		pos.setCoords(SelectedFishPos.X() + camDist * cos(fi2) * cos(fi1),
			SelectedFishPos.Y() + camDist * cos(fi2) * sin(fi1),
			SelectedFishPos.Z() + camDist * sin(fi2));

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

	void SetLookPoint(double x, double y, double z)
	{
		lookPoint.setCoords(x, y, z);
	}

	void SetLookPoint(Vector3 vec)
	{
		lookPoint = vec;
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
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
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
		/*
		if (f1)
			glEnable(GL_LIGHTING);
		if (f2)
			glEnable(GL_TEXTURE_2D);
		if (f3)
			glEnable(GL_DEPTH_TEST);
			*/
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
}
light;  //������� �������� �����
//������ ���������� ����
int mouseX = 0, mouseY = 0;
float offsetX = 0, offsetY = 0;
float zoom=1;

//���������� �������� ����
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.005 * dx;
		camera.fi2 += -0.005 * dy;
		camera.pos.setCoords(camera.camDist * cos(camera.fi2) * cos(camera.fi1),
			camera.camDist * cos(camera.fi2) * sin(camera.fi1),
			camera.camDist * sin(camera.fi2));
		//camera.camDist = 10;
		

	}


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}
	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y,60,ogl->aspect);

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

//���������� �������� ������  ����
void mouseWheelEvent(OpenGL *ogl, int delta)
{
	camera.camDist += 0.005 * delta;
	camera.pos.setCoords(camera.camDist * cos(camera.fi2) * cos(camera.fi1),
		camera.camDist * cos(camera.fi2) * sin(camera.fi1),
		camera.camDist * sin(camera.fi2));
}

//���������� ������� ������ ����������
void keyDownEvent(OpenGL *ogl, int key)
{
	if (OpenGL::isKeyPressed('L'))
	{
		lightMode = !lightMode;
	}

	if (OpenGL::isKeyPressed('T'))
	{
		textureMode = !textureMode;
	}	   

	if (OpenGL::isKeyPressed('F'))
	{
		light.pos = camera.pos;
	}

	if (OpenGL::isKeyPressed('Q') && selector != YourFish)
	{
		if (selector != Plotva1)
			selector = (SelectFish)(selector - 1);
	}

	if (OpenGL::isKeyPressed('E') && selector != YourFish)
	{
		if (selector != Dora3)
			selector = (SelectFish)(selector + 1);
	}

	if (OpenGL::isKeyPressed('E') && selector == YourFish)
	{
		if (selectorForYourFish != Dor)
			selectorForYourFish = (SelectYourFish)(selectorForYourFish + 1);
	}

	if (OpenGL::isKeyPressed('Q') && selector == YourFish)
	{
		if (selectorForYourFish != Plotv)
			selectorForYourFish = (SelectYourFish)(selectorForYourFish - 1);
	}

	if (OpenGL::isKeyPressed('X'))
	{
		selector = YourFish;
	}

	if (OpenGL::isKeyPressed('Z'))
	{
		selector = Clown2;
	}

}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}

PlotvaFish plotva;
TunaFish Tuna;
ClownFish Clown;
DoraFish Dora;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	srand(time(NULL));

	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);

	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 
	
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	/*
	//texture1.loadTextureFromFile("textures\\texture.bmp");   �������� �������� �� �����
	*/


	frac.VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	frac.FshaderFileName = "shaders\\frac.frag"; //��� ����� ������������ �������
	frac.LoadShaderFromFile(); //��������� ������� �� �����
	frac.Compile(); //�����������

	cassini.VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	cassini.FshaderFileName = "shaders\\cassini.frag"; //��� ����� ������������ �������
	cassini.LoadShaderFromFile(); //��������� ������� �� �����
	cassini.Compile(); //�����������
	

	s[0].VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	s[0].FshaderFileName = "shaders\\light.frag"; //��� ����� ������������ �������
	s[0].LoadShaderFromFile(); //��������� ������� �� �����
	s[0].Compile(); //�����������

	s[1].VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	s[1].FshaderFileName = "shaders\\textureShader.frag"; //��� ����� ������������ �������
	s[1].LoadShaderFromFile(); //��������� ������� �� �����
	s[1].Compile(); //�����������

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Temp_Plotva_withoutPlavnik.obj_m", &(plotva.Fish));
	loadModel("models\\Temp_Plotva_DownPlavnik.obj_m", &(plotva.DownPlavnik));
	loadModel("models\\Temp_Plotva_UpPlavnik.obj_m", &(plotva.UpperPlavnik));
	loadModel("models\\Temp_Plotva_XBOST.obj_m", &(plotva.Hvost));
	plotva.FishTex.loadTextureFromFile("textures//fish_texture.bmp");

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\TunaWihoutPlavnik.obj_m", &(Tuna.Fish));
	loadModel("models\\TunaPlavnikCenter.obj_m", &(Tuna.PlavnikCenter));
	loadModel("models\\TunaCenterR.obj_m", &(Tuna.PlavnikCenterR));
	loadModel("models\\TunaPlavnikUp.obj_m", &(Tuna.PlavnikUpper));
	loadModel("models\\TunaPlavnikDown.obj_m", &(Tuna.PlavnikDown));
	loadModel("models\\TunaDownR.obj_m", &(Tuna.PlavnikDownR));
	loadModel("models\\TunaPlavnikDownRear.obj_m", &(Tuna.PlavnikDownRear));
	loadModel("models\\TunaPlavnikHvost.obj_m", &(Tuna.Hvost));
	Tuna.FishTex.loadTextureFromFile("textures//Tuna.bmp");

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Clown\\ClownFishWithoutPlavnik.obj_m", &(Clown.Fish));
	loadModel("models\\Clown\\ClownFishCenterL.obj_m", &(Clown.PlavnikCenter));
	loadModel("models\\Clown\\ClownFishCenterR.obj_m", &(Clown.PlavnikCenterR));
	loadModel("models\\Clown\\ClownFishPlavnikUpper.obj_m", &(Clown.PlavnikUpper));
	loadModel("models\\Clown\\ClownFishDownL.obj_m", &(Clown.PlavnikDown));
	loadModel("models\\Clown\\ClownFishDownR.obj_m", &(Clown.PlavnikDownR));
	loadModel("models\\Clown\\ClownFishDownRear.obj_m", &(Clown.PlavnikDownRear));
	loadModel("models\\Clown\\ClownFishHvost.obj_m", &(Clown.Hvost));
	Clown.FishTex.loadTextureFromFile("textures//ClownFish.bmp");

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Dora\\DoraWithOutPlavnik.obj_m", &(Dora.Fish));
	loadModel("models\\Dora\\DoraCenterL.obj_m", &(Dora.PlavnikCenter));
	loadModel("models\\Dora\\DoraCenterR.obj_m", &(Dora.PlavnikCenterR));
	loadModel("models\\Dora\\DoraUpp.obj_m", &(Dora.PlavnikUpper));
	loadModel("models\\Dora\\DoraDownRear.obj_m", &(Dora.PlavnikDownRear));
	loadModel("models\\Dora\\DoraHvost.obj_m", &(Dora.Hvost));
	Dora.FishTex.loadTextureFromFile("textures//Dora.bmp");

	 //��� ��� ��� ������� ������ *.obj �����, ��� ��� ��� ��������� �� ���������� � ���������� �������, 
	 // ������������ �� ����� ����������, � ������������ ������ � *.obj_m

	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("textures//Fon.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//���������� �� ��� ��������
	glGenTextures(1, &texId[0]);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId[0]);

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


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

}

void DrawAllFishes()
{
	//���������� ���������� �����, �� ������� ������
	PUSH;
	if(selector == Plotva1)
		SelectedFishPos = plotva.BizeGo();
	else
		plotva.BizeGo();
	if (selector == Plotva2)
		SelectedFishPos = plotva.BizeGo2();
	else
		plotva.BizeGo2();
	if (selector == Plotva3)
		SelectedFishPos = plotva.BizeGo3();
	else
		plotva.BizeGo3();
	if (selector == Tuna1)
		SelectedFishPos = Tuna.BizeGo();
	else
		Tuna.BizeGo();
	if (selector == Tuna2)
		SelectedFishPos = Tuna.BizeGo2();
	else
		Tuna.BizeGo2();
	if (selector == Tuna3)
		SelectedFishPos = Tuna.BizeGo3();
	else
		Tuna.BizeGo3();
	if (selector == Clown1)
		SelectedFishPos = Clown.BizeGo();
	else
		Clown.BizeGo();
	if (selector == Clown2)
		SelectedFishPos = Clown.BizeGo2();
	else
		Clown.BizeGo2();
	if (selector == Clown3)
		SelectedFishPos = Clown.BizeGo3();
	else
		Clown.BizeGo3();
	if (selector == Dora1)
		SelectedFishPos = Dora.BizeGo();
	else
		Dora.BizeGo();
	if (selector == Dora2)
		SelectedFishPos = Dora.BizeGo2();
	else
		Dora.BizeGo2();
	if (selector == Dora3)
		SelectedFishPos = Dora.BizeGo3();
	else
		Dora.BizeGo3();
	//������ ���� �����
	if (selector == YourFish)
	{
		if (selectorForYourFish == Plotv)
		{
			PUSH;
			glTranslated(YourPos.X(), YourPos.Y(), YourPos.Z());
			glScaled(0.3, 0.3, 0.3);
			glScaled(2, 2, 2);
			glRotated(Angle, 0, 0, 1);
			plotva.DrawFish();
			POP;
		}
		if (selectorForYourFish == Tun)
		{
			PUSH;
			glTranslated(YourPos.X(), YourPos.Y(), YourPos.Z());
			glScaled(2, 2, 2);
			glRotated(Angle, 0, 0, 1);
			Tuna.DrawFish();
			POP;
		}
		if (selectorForYourFish == Clow)
		{
			PUSH;
			glTranslated(YourPos.X(), YourPos.Y(), YourPos.Z());
			glScaled(1.5, 1.5, 1.5);
			glScaled(2, 2, 2);
			glRotated(Angle, 0, 0, 1);
			Clown.DrawFish();
			POP;
		}
		if (selectorForYourFish == Dor)
		{
			PUSH;
			glTranslated(YourPos.X(), YourPos.Y(), YourPos.Z());
			glScaled(1.5, 1.5, 1.5);
			glScaled(2, 2, 2);
			glRotated(Angle, 0, 0, 1);
			Dora.DrawFish();
			POP;
		}
		SelectedFishPos = YourPos;
	}
	POP;
}

void MyKeyIvent() //����� ��� ������� �����
{
	if (selector == YourFish)
	{
		if (OpenGL::isKeyPressed('W'))
		{
			YourPos.SetCoordX(YourPos.X() + 0.1 * cos(Angle * PI / 180));
			YourPos.SetCoordY(YourPos.Y() + 0.1 * sin(Angle * PI / 180));
		}

		if (OpenGL::isKeyPressed('S'))
		{
			YourPos.SetCoordX(YourPos.X() - 0.1 * cos(Angle * PI / 180));
			YourPos.SetCoordY(YourPos.Y() - 0.1 * sin(Angle * PI / 180));
		}

		if (OpenGL::isKeyPressed('A'))
		{
			Angle += 1;
		}

		if (OpenGL::isKeyPressed('D'))
		{
			Angle -= 1;
		}

		if (OpenGL::isKeyPressed(16))
		{
			YourPos.SetCoordZ(YourPos.Z() + 0.05);
		}

		if (OpenGL::isKeyPressed(17))
		{
			YourPos.SetCoordZ(YourPos.Z() - 0.05);
		}
	}
}

void Render(OpenGL *ogl)
{   
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);



	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//��������� ���������
	GLfloat amb[] = { 0.6, 0.6, 0.6, 1. };
	GLfloat dif[] = { 0.9, 0.9, 0.9, 1. };
	GLfloat spec[] = { 0.9, 0.9, 0.9, 1. };
	GLfloat sh = 0.1f * 256;

	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//������� ���  




	s[0].UseShader();

	//�������� ���������� � ������.  ��� ���� - ���� ����� uniform ���������� �� �� �����. 
	int location = glGetUniformLocationARB(s[0].program, "light_pos");
	//��� 2 - �������� �� ��������
	glUniform3fARB(location, light.pos.X(), light.pos.Y(),light.pos.Z());

	location = glGetUniformLocationARB(s[0].program, "Ia");
	glUniform3fARB(location, 0.2, 0.2, 0.2);

	location = glGetUniformLocationARB(s[0].program, "Id");
	glUniform3fARB(location, 1.0, 1.0, 1.0);

	location = glGetUniformLocationARB(s[0].program, "Is");
	glUniform3fARB(location, .7, .7, .7);


	location = glGetUniformLocationARB(s[0].program, "ma");
	glUniform3fARB(location, 0.2, 0.2, 0.1);

	location = glGetUniformLocationARB(s[0].program, "md");
	glUniform3fARB(location, 0.4, 0.65, 0.5);

	location = glGetUniformLocationARB(s[0].program, "ms");
	glUniform4fARB(location, 0.9, 0.8, 0.3, 25.6);

	location = glGetUniformLocationARB(s[0].program, "camera");
	glUniform3fARB(location, camera.pos.X(), camera.pos.Y(), camera.pos.Z());

	Shader::DontUseShaders();
	
	//�����
	//PUSH;
	//glScaled(0.3, 0.3, 0.3);
	//plotva.DrawFish();
	//POP;
	//PUSH;
	//glTranslated(2, 0, 0);
	//Tuna.DrawFish();
	//POP;
	//PUSH;
	//glTranslated(4, 0, 0);
	//glScaled(1.5, 1.5, 1.5);
	//Clown.DrawFish();
	//POP;
	//PUSH;
	//glTranslated(6, 0, 0);
	//glScaled(1.5, 1.5, 1.5);
	//Dora.DrawFish();
	//POP;
	//------

	MyKeyIvent();
	DrawAllFishes();

	double B[] = { 25, -1, 25 };
	double C[] = { 25, -1, -2 };
	double D[] = { -25, -1,  -2 };
	double E[] = { -25, -1, 25 };
	
	glPushMatrix();
	
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	glBegin(GL_QUADS);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(B);
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(C);
	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(D);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(E);
	
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
	glColor4d(66/255., 170/255., 1, 0.6);
	//����� ������
	glVertex3d(25, -1, 25);
	glVertex3d(25, -1, -2);
	glVertex3d(25, 15, -2);
	glVertex3d(25, 15, 25);
	//������ ������
	glVertex3d(-25, -1, 25);
	glVertex3d(-25, -1, -2);
	glVertex3d(-25, 15, -2);
	glVertex3d(-25, 15, 25);
	//���
	glColor4d(66 / 255., 170 / 255., 1, 0.9);
	glVertex3d(-25, -1, -2);
	glVertex3d(-25, 15, -2);
	glVertex3d(25, 15, -2);
	glVertex3d(25, -1, -2);
	//����
	glColor4d(66 / 255., 170 / 255., 1, 0.4);
	glVertex3d(-25, -1, 25);
	glVertex3d(-25, 15, 25);
	glVertex3d(25, 15, 25);
	glVertex3d(25, -1, 25);
	//�����
	glColor4d(66 / 255., 170 / 255., 1, 0.2);
	glVertex3d(25, 15, 25);
	glVertex3d(25, 15, -2);
	glVertex3d(-25, 15, -2);
	glVertex3d(-25, 15, 25);
	glEnd();

}   //����� ���� �������


bool gui_init = false;

//������ ���������, ��������� ����� �������� �������
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

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
	rec.setSize(300, 250);
	rec.setPosition(10, ogl->getHeight() - 250 - 10);


	std::stringstream ss;

	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "Q, E - ����� �����" << std::endl;
	ss << "----- ���������� ����� ������ --------" << std::endl;
	ss << "X, Z - ���/���� ���� �����" << std::endl;
	ss << "Q, E - ����� ��������" << std::endl;
	ss << "WASD - ���������� �� XY\nShift, Ctrl - ����/����" << std::endl;
	ss << "------------------------------------------------" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;


	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
		
	Shader::DontUseShaders(); 
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

