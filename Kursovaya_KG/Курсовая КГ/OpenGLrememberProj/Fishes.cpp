#include "Fishes.h"
#include "MyOGL.h"

double f(double p1, double p2, double p3, double p4, double t)
{
	return p1 * (1 - t) * (1 - t) * (1 - t) + 3 * p2 * t * (1 - t) * (1 - t) + 3 * p3 * t * t * (1 - t) + p4 * t * t * t; //посчитанная формула
}

void bize(double P1[3], double P2[3], double P3[3], double P4[3])
{
	glDisable(GL_LIGHTING);
	glColor3d(1, 0.5, 0);
	glLineWidth(3); //ширина линии
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		double P[3];
		P[0] = f(P1[0], P2[0], P3[0], P4[0], t);
		P[1] = f(P1[1], P2[1], P3[1], P4[1], t);
		P[2] = f(P1[2], P2[2], P3[2], P4[2], t);
		glVertex3dv(P); //Рисуем точку P
	}
	glEnd();
	glColor3d(0, 0, 1);
	glLineWidth(1); //возвращаем ширину линии = 1

	glBegin(GL_LINES);
	glVertex3dv(P1);
	glVertex3dv(P2);
	glVertex3dv(P2);
	glVertex3dv(P3);
	glVertex3dv(P3);
	glVertex3dv(P4);
	glEnd();
	glEnable(GL_LIGHTING);
}

Vector3 bizeWithoutDraw(double P1[3], double P2[3], double P3[3], double P4[3], double t)
{
	Vector3 Vec;
	Vec.setCoords(f(P1[0], P2[0], P3[0], P4[0], t), f(P1[1], P2[1], P3[1], P4[1], t), f(P1[2], P2[2], P3[2], P4[2], t));
	return Vec;
}

double Pvsevdo_Random(double Low, double Up)
{
	return Low + (Up - Low) * (static_cast<double>(rand()) / RAND_MAX);
}

double Pvsevdo_Random_Speed(double delta_time)
{
	double dV = Pvsevdo_Random(-0.0005, 0.0005); // Приращение скорости
	double newDelta = 0;
	newDelta = delta_time + dV; // Псевдорандомная скорость рыбки
	if (newDelta >= 0.013) // Границы скорости
	{
		newDelta = 0.013;
	}
	if (newDelta <= 0.004)
	{
		newDelta = 0.004;
	}
	return newDelta;
}

double Pvsevdo_Random_Z(double oldZ)
{
	double dZ = Pvsevdo_Random(-0.5, 0.5); // Приращение координаты
	double newZ = 0;
	newZ = oldZ + dZ; // Псевдорандомная скорость рыбки
	if (newZ >= 19) // Границы скорости
	{
		newZ = 19;
	}
	if (newZ <= 1)
	{
		newZ = 1;
	}
	return newZ;
}

void PlotvaFish::DrawFish()
{
	if (!reverse)
	{
		if (angle + delta > 25)
		{
			angle = 25;
			reverse = true;
		}
		else
		{
			angle += delta;
		}
	}
	else
	{
		if (angle - delta < 0)
		{
			angle = 0;
			reverse = false;
		}
		else
		{
			angle -= delta;
		}
	}

	if (!reverseUp)
	{
		if (angleUp + delta > 10)
		{
			angleUp = 10;
			reverseUp = true;
		}
		else
		{
			angleUp += delta;
		}
	}
	else
	{
		if (angleUp - delta < -10)
		{
			angleUp = -10;
			reverseUp = false;
		}
		else
		{
			angleUp -= delta;
		}
	}

	glPushMatrix();

	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	FishTex.bindTexture();
	Fish.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0.95, 0.1, -0.2);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angle, 0, 0, 1);
	DownPlavnik.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0.95, -0.1, -0.2);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(-angle, 0, 0, 1);
	DownPlavnik.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(-0.1, 0.08, -0.35);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angle, 0, 0, 1);
	DownPlavnik.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(-0.1, -0.08, -0.35);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(-angle, 0, 0, 1);
	DownPlavnik.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(-0.21, 0, 0.57);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(-6, 1, 0, 0);
	glRotated(angleUp, 0, 0, 1);
	UpperPlavnik.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(-1.9, 0, 0.02);
	glScaled(0.75, 0.75, 0.75);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 1, 0);
	Hvost.DrawObj();

	glPopMatrix();
}

Vector3 PlotvaFish::BizeGo()
{
	static double P1[] = { 19,1,4 }; //Наши точки
	static double P2[] = { 2,3,1 };
	static double P3[] = { -10,1,3 };
	static double P4[] = { -19,7,2 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(0.3, 0.3, 0.3);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

Vector3 PlotvaFish::BizeGo2()
{
	static double P1[] = { 18,1,13 }; //Наши точки
	static double P2[] = { 9,5,11 };
	static double P3[] = { -9,2,9 };
	static double P4[] = { -19,4,8 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(0.3, 0.3, 0.3);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

Vector3 PlotvaFish::BizeGo3()
{
	static double P1[] = { 19,7,13 }; //Наши точки
	static double P2[] = { 2,1,13 };
	static double P3[] = { -10,2,14 };
	static double P4[] = { -19,3,17 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(0.3, 0.3, 0.3);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

void TunaFish::DrawFish()
{
	if (!reverse)
	{
		if (angle + delta > 25)
		{
			angle = 25;
			reverse = true;
		}
		else
		{
			angle += delta;
		}
	}
	else
	{
		if (angle - delta < 0)
		{
			angle = 0;
			reverse = false;
		}
		else
		{
			angle -= delta;
		}
	}

	if (!reverseUp)
	{
		if (angleUp + delta > 10)
		{
			angleUp = 10;
			reverseUp = true;
		}
		else
		{
			angleUp += delta;
		}
	}
	else
	{
		if (angleUp - delta < -10)
		{
			angleUp = -10;
			reverseUp = false;
		}
		else
		{
			angleUp -= delta;
		}
	}

	glPushMatrix();

	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	FishTex.bindTexture();
	Fish.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0.26, 0.1, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(-20, 0, 1, 0);
	glRotated(angle, 0, 1, 0);
	PlavnikCenter.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0.25, -0.1, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(20, 0, 1, 0);
	glRotated(-angle, 0, 1, 0);
	PlavnikCenterR.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, 0, 0.195);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 0, 1);
	PlavnikUpper.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, 0.075, -0.23);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 0, 1);
	PlavnikDown.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, -0.09, -0.24);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 0, 1);
	PlavnikDownR.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0.05, 0, -0.13);
	glScaled(1.2, 1.2, 1.2);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 0, 1);
	PlavnikDownRear.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(-0.72, 0, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 1, 0);
	Hvost.DrawObj();

	glPopMatrix();
}

Vector3 TunaFish::BizeGo()
{
	static double P1[] = { 19,4,6 }; //Наши точки
	static double P2[] = { 5,2,5 };
	static double P3[] = { -12,6,7 };
	static double P4[] = { -19,0.5,9 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

Vector3 TunaFish::BizeGo2()
{
	static double P1[] = { 18,1,11 }; //Наши точки
	static double P2[] = { 9,2,6 };
	static double P3[] = { -9,1,9 };
	static double P4[] = { -19,3,13 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

Vector3 TunaFish::BizeGo3()
{
	static double P1[] = { 18,2,13 }; //Наши точки
	static double P2[] = { 9,1,16 };
	static double P3[] = { -9,1,13 };
	static double P4[] = { -19,2,16 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

void ClownFish::DrawFish()
{
	if (!reverse)
	{
		if (angle + delta > 25)
		{
			angle = 25;
			reverse = true;
		}
		else
		{
			angle += delta;
		}
	}
	else
	{
		if (angle - delta < 0)
		{
			angle = 0;
			reverse = false;
		}
		else
		{
			angle -= delta;
		}
	}

	if (!reverseUp)
	{
		if (angleUp + delta > 10)
		{
			angleUp = 10;
			reverseUp = true;
		}
		else
		{
			angleUp += delta;
		}
	}
	else
	{
		if (angleUp - delta < -10)
		{
			angleUp = -10;
			reverseUp = false;
		}
		else
		{
			angleUp -= delta;
		}
	}

	glPushMatrix();

	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	FishTex.bindTexture();
	Fish.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0.06, 0.03, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(-21, 0, 1, 0);
	glRotated(angle, 0, 1, 0);
	PlavnikCenter.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0.06, -0.03, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(21, 0, 1, 0);
	glRotated(-angle, 0, 1, 0);
	PlavnikCenterR.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, 0, 0.06);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(0, 0, 1, 0);
	glRotated(angleUp, 0, 0, 1);
	PlavnikUpper.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, 0.02, 0.002);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp / 6, 0, 0, 1);
	PlavnikDown.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, -0.02, 0.002);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp / 6, 0, 0, 1);
	PlavnikDownR.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, 0, -0.04);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 0, 1);
	PlavnikDownRear.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(-0.21, 0, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 1, 0);
	Hvost.DrawObj();

	glPopMatrix();
}

Vector3 ClownFish::BizeGo()
{
	static double P1[] = { 18,3,13 }; //Наши точки
	static double P2[] = { 9,4,11 };
	static double P3[] = { -9,3.5,9 };
	static double P4[] = { -19,5,8 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(1.5, 1.5, 1.5);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

Vector3 ClownFish::BizeGo2()
{
	static double P1[] = { 19,4,1 }; //Наши точки
	static double P2[] = { 5,5,5 };
	static double P3[] = { -12,6,7 };
	static double P4[] = { -19,1,9 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(1.5, 1.5, 1.5);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

Vector3 ClownFish::BizeGo3()
{
	static double P1[] = { 19,4,15 }; //Наши точки
	static double P2[] = { 8,2,17 };
	static double P3[] = { -9,6,13 };
	static double P4[] = { -19,3,14 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(1.5, 1.5, 1.5);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

void DoraFish::DrawFish()
{
	if (!reverse)
	{
		if (angle + delta > 25)
		{
			angle = 25;
			reverse = true;
		}
		else
		{
			angle += delta;
		}
	}
	else
	{
		if (angle - delta < 0)
		{
			angle = 0;
			reverse = false;
		}
		else
		{
			angle -= delta;
		}
	}

	if (!reverseUp)
	{
		if (angleUp + delta > 10)
		{
			angleUp = 10;
			reverseUp = true;
		}
		else
		{
			angleUp += delta;
		}
	}
	else
	{
		if (angleUp - delta < -10)
		{
			angleUp = -10;
			reverseUp = false;
		}
		else
		{
			angleUp -= delta;
		}
	}

	glPushMatrix();

	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	FishTex.bindTexture();
	Fish.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, 0.04, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(-20, 0, 1, 0);
	glRotated(angle, 0, 1, 0);
	PlavnikCenter.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, -0.04, 0);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(20, 0, 1, 0);
	glRotated(-angle, 0, 1, 0);
	PlavnikCenterR.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0, 0, 0.111);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp / 3, 0, 0, 1);
	PlavnikUpper.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(0.07, 0, 0.08);
	glScaled(1.2, 1.2, 1.2);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(-5, 1, 0, 0);
	glRotated(angleUp / 3, 0, 0, 1);
	PlavnikDownRear.DrawObj();

	glPopMatrix();

	glPushMatrix();

	glTranslated(-0.38, 0, 0.08);
	glRotated(90, 0, 1, 0);
	glRotated(90, 0, 0, 1);
	glRotated(angleUp, 0, 1, 0);
	Hvost.DrawObj();

	glPopMatrix();
}

Vector3 DoraFish::BizeGo()
{
	static double P1[] = { 19,1,15 }; //Наши точки
	static double P2[] = { 2,3,18 };
	static double P3[] = { -10,1,14 };
	static double P4[] = { -19,7,16 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(1.5, 1.5, 1.5);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

Vector3 DoraFish::BizeGo2()
{
	static double P1[] = { 19,4,4 }; //Наши точки
	static double P2[] = { -4,5,1 };
	static double P3[] = { -8,6,3 };
	static double P4[] = { -19,5,2 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(1.5, 1.5, 1.5);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}

Vector3 DoraFish::BizeGo3()
{
	static double P1[] = { 19,1,11 }; //Наши точки
	static double P2[] = { 1,2,12 };
	static double P3[] = { -9,1,13 };
	static double P4[] = { -19,2,10 };
	static double delta_time = 0.01;
	delta_time = Pvsevdo_Random_Speed(delta_time);

	static double t_max = 0;
	static bool flagReverse = false;

	if (!flagReverse)
	{
		t_max += delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max > 1)
		{
			t_max = 1; //после обнуляется
			flagReverse = !flagReverse;
			P1[2] = Pvsevdo_Random_Z(P1[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}
	else
	{
		t_max -= delta_time / 5; //t_max становится = 1 за 5 секунд
		if (t_max < 0)
		{
			t_max = 0; //после обнуляется
			flagReverse = !flagReverse;
			P4[2] = Pvsevdo_Random_Z(P4[2]);
			P2[2] = Pvsevdo_Random_Z(P2[2]);
			P3[2] = Pvsevdo_Random_Z(P3[2]);
		}
	}

	//bize(P1, P2, P3, P4);

	Vector3 P_old = bizeWithoutDraw(P1, P2, P3, P4, !flagReverse ? t_max - delta_time : t_max + delta_time);
	Vector3 P = bizeWithoutDraw(P1, P2, P3, P4, t_max);
	Vector3 VecP_P_old = (P - P_old).normolize();

	Vector3 rotateX(VecP_P_old.X(), VecP_P_old.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VecPrX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double CosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double SinAngleZ = VecPrX.Z() / abs(VecPrX.Z());
	double AngleOZ = acos(CosX) * 180 / PI * SinAngleZ;

	double AngleOY = acos(VecP_P_old.Z()) * 180 / PI - 90;

	glPushMatrix();
	glTranslated(P.X(), P.Y(), P.Z());
	glScaled(1.5, 1.5, 1.5);
	glScaled(2, 2, 2);
	glRotated(AngleOZ, 0, 0, 1);
	glRotated(AngleOY, 0, 1, 0);
	DrawFish();
	glPopMatrix();
	return P;
}
