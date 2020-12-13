#ifndef Fishes_H
#define Fishes_H

#include "MyVector3d.h"
#include "MyOGL.h"
#include "Texture.h"
#include "ObjLoader.h"


class PlotvaFish
{
	double delta = 1;
	double angle = 0;
	double angleUp = 0;
	bool reverse = false;
	bool reverseUp = false;

public:
	ObjFile Fish, DownPlavnik, UpperPlavnik, Hvost;
	Texture FishTex;
	void DrawFish();
	Vector3 BizeGo();
	Vector3 BizeGo2();
	Vector3 BizeGo3();
};

class TunaFish
{
	double delta = 1;
	double angle = 0;
	double angleUp = 0;
	bool reverse = false;
	bool reverseUp = false;

public:
	ObjFile Fish, PlavnikCenter, PlavnikCenterR, PlavnikUpper, PlavnikDown, PlavnikDownR, PlavnikDownRear, Hvost;
	Texture FishTex;
	void DrawFish();
	Vector3 BizeGo();
	Vector3 BizeGo2();
	Vector3 BizeGo3();
};

class ClownFish
{
	double delta = 1;
	double angle = 0;
	double angleUp = 0;
	bool reverse = false;
	bool reverseUp = false;

public:
	ObjFile Fish, PlavnikCenter, PlavnikCenterR, PlavnikUpper, PlavnikDown, PlavnikDownR, PlavnikDownRear, Hvost;
	Texture FishTex;
	void DrawFish();
	Vector3 BizeGo();
	Vector3 BizeGo2();
	Vector3 BizeGo3();
};

class DoraFish
{
	double delta = 1;
	double angle = 0;
	double angleUp = 0;
	bool reverse = false;
	bool reverseUp = false;

public:
	ObjFile Fish, PlavnikCenter, PlavnikCenterR, PlavnikUpper, PlavnikDownRear, Hvost;
	Texture FishTex;
	void DrawFish();
	Vector3 BizeGo();
	Vector3 BizeGo2();
	Vector3 BizeGo3();
};

#endif