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
//небольшой дефайн для упрощения кода
#define POP glPopMatrix()
#define PUSH glPushMatrix()
ObjFile *model;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

Shader s[10];  //массивчик для десяти шейдеров
Shader frac;
Shader cassini;

enum SelectFish //Перечисление для выбора рыбок
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
	YourFish // Рыбка для управления с клавиатуры
};

enum SelectYourFish
{
	Plotv,
	Tun,
	Clow,
	Dor
};

SelectFish selector; // Для выбора, за какой рыбкой наблюдаем

SelectYourFish selectorForYourFish; // Выбор модельки, которой управляем

Vector3 SelectedFishPos; //Координаты выбранной рыбки

Vector3 YourPos; //Позиция управляемой рыбки
double Angle = 0; //Угол управляемой рыбки

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	Vector3 cameraFront;

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
		//функция настройки камеры
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
}
light;  //создаем источник света
//старые координаты мыши
int mouseX = 0, mouseY = 0;
float offsetX = 0, offsetY = 0;
float zoom=1;

//обработчик движения мыши
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
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
	
	//двигаем свет по плоскости, в точку где мышь
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

//обработчик вращения колеса  мыши
void mouseWheelEvent(OpenGL *ogl, int delta)
{
	camera.camDist += 0.005 * delta;
	camera.pos.setCoords(camera.camDist * cos(camera.fi2) * cos(camera.fi1),
		camera.camDist * cos(camera.fi2) * sin(camera.fi1),
		camera.camDist * sin(camera.fi2));
}

//обработчик нажатия кнопок клавиатуры
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

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	srand(time(NULL));

	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);

	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 
	
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	/*
	//texture1.loadTextureFromFile("textures\\texture.bmp");   загрузка текстуры из файла
	*/


	frac.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	frac.FshaderFileName = "shaders\\frac.frag"; //имя файла фрагментного шейдера
	frac.LoadShaderFromFile(); //загружаем шейдеры из файла
	frac.Compile(); //компилируем

	cassini.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	cassini.FshaderFileName = "shaders\\cassini.frag"; //имя файла фрагментного шейдера
	cassini.LoadShaderFromFile(); //загружаем шейдеры из файла
	cassini.Compile(); //компилируем
	

	s[0].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[0].FshaderFileName = "shaders\\light.frag"; //имя файла фрагментного шейдера
	s[0].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[0].Compile(); //компилируем

	s[1].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[1].FshaderFileName = "shaders\\textureShader.frag"; //имя файла фрагментного шейдера
	s[1].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[1].Compile(); //компилируем

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

	 //так как гит игнорит модели *.obj файлы, так как они совпадают по расширению с объектными файлами, 
	 // создающимися во время компиляции, я переименовал модели в *.obj_m

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE* texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("textures//Fon.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//генерируем ИД для текстуры
	glGenTextures(1, &texId[0]);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId[0]);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

}

void DrawAllFishes()
{
	//Возвращаем координаты рыбки, за которой следим
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
	//Рисуем нашу рыбку
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

void MyKeyIvent() //ивент для каждого кадра
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

	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//настройка материала
	GLfloat amb[] = { 0.6, 0.6, 0.6, 1. };
	GLfloat dif[] = { 0.9, 0.9, 0.9, 1. };
	GLfloat spec[] = { 0.9, 0.9, 0.9, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//Прогать тут  




	s[0].UseShader();

	//передача параметров в шейдер.  Шаг один - ищем адрес uniform переменной по ее имени. 
	int location = glGetUniformLocationARB(s[0].program, "light_pos");
	//Шаг 2 - передаем ей значение
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
	
	//Тесты
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
	//Левая стенка
	glVertex3d(25, -1, 25);
	glVertex3d(25, -1, -2);
	glVertex3d(25, 15, -2);
	glVertex3d(25, 15, 25);
	//Правая стенка
	glVertex3d(-25, -1, 25);
	glVertex3d(-25, -1, -2);
	glVertex3d(-25, 15, -2);
	glVertex3d(-25, 15, 25);
	//Низ
	glColor4d(66 / 255., 170 / 255., 1, 0.9);
	glVertex3d(-25, -1, -2);
	glVertex3d(-25, 15, -2);
	glVertex3d(25, 15, -2);
	glVertex3d(25, -1, -2);
	//Верх
	glColor4d(66 / 255., 170 / 255., 1, 0.4);
	glVertex3d(-25, -1, 25);
	glVertex3d(-25, 15, 25);
	glVertex3d(25, 15, 25);
	glVertex3d(25, -1, 25);
	//Перед
	glColor4d(66 / 255., 170 / 255., 1, 0.2);
	glVertex3d(25, 15, 25);
	glVertex3d(25, 15, -2);
	glVertex3d(-25, 15, -2);
	glVertex3d(-25, 15, 25);
	glEnd();

}   //конец тела функции


bool gui_init = false;

//рисует интерфейс, вызывется после обычного рендера
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

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
	rec.setSize(300, 250);
	rec.setPosition(10, ogl->getHeight() - 250 - 10);


	std::stringstream ss;

	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "Q, E - Выбор рыбки" << std::endl;
	ss << "----- Управление своей рыбкой --------" << std::endl;
	ss << "X, Z - Вкл/выкл свою рыбку" << std::endl;
	ss << "Q, E - Выбор модельки" << std::endl;
	ss << "WASD - Управление по XY\nShift, Ctrl - Выше/Ниже" << std::endl;
	ss << "------------------------------------------------" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;


	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
		
	Shader::DontUseShaders(); 
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

