#include "Algorithm.h"
#include "MyMathLib.h"

int Algorithm::GetParabola(int x)
{
	return (int)pow(x, 2);
}

void Algorithm::GetParabola(int& x, int& y, float t)
{
	x = (int)t;
	y = (int)pow((int)t, 2);
}

void Algorithm::GetCircle(int& x, int&y, float t)
{
    //float t2 = Math::DEGREES_TO_RADIANS * t;
	x = (int)cos(t);
	y = (int)sin(t);
}


void Algorithm::GetEllipse(int& x, int& y, float t, int a, int b)
{
	x = a * (int)cos(t);
	y = b * (int)sin(t);
}

void Algorithm::GetLissajousCurve(int& x, int& y, float t, int a, int b, int xLobes, int yLobes)
{
	x = a * (int)cos(xLobes * t);
	y = b * (int)sin(yLobes * t);
}

void Algorithm::GetHypotrochoid(int& x, int&y, float t, int bigR, int r, int d)
{
	int tX = 0, tY = 0;
	int distanceFromOrigo = bigR - r;
	GetCircle(tX, tY, t);
	//Apply the distance
	tX *= distanceFromOrigo;
	tY *= distanceFromOrigo;
	//Angle between origin and innerCircleOrigin.
	double angle = atan2(tX, tY);
	//Calculate the local x and y
	x = distanceFromOrigo * (int)cos(angle) + d * (int)cos((distanceFromOrigo / r) * angle);
	y = distanceFromOrigo * (int)sin(angle) - d * (int)sin((distanceFromOrigo / r) * angle);
}

void Algorithm::GetParabola(float& x, float& y, float t)
{
	x = t;
	y = x * x;
}

void Algorithm::GetCircle(float& x, float&y, float t)
{
	x = cos(t);
	y = sin(t);
}


void Algorithm::GetEllipse(float& x, float& y, float t, float a, float b)
{
	x = a * cos(t);
	y = b * sin(t);
}

void Algorithm::GetLissajousCurve(float& x, float& y, float t, float a, float b, float xLobes, float yLobes)
{
	x = a * cos(xLobes * t);
	y = b * sin(yLobes * t);
}

void Algorithm::GetHypotrochoid(float& x, float&y, float t, float bigR, float r, float d)
{
	float tX = 0, tY = 0;
	float distanceFromOrigo = bigR - r;
	GetCircle(tX, tY, t);
	//Apply the distance
	tX *= distanceFromOrigo;
	tY *= distanceFromOrigo;
	//Angle between origin and innerCircleOrigin.
	float angle = atan2(tY - 0.0f, tX - 0.0f);
	//Calculate the local x and y
	x = distanceFromOrigo * cos(angle) + d * cos((distanceFromOrigo / r) * angle);
	y = distanceFromOrigo * sin(angle) - d * sin((distanceFromOrigo / r) * angle);
	/*x = tX;
	y = tY;*/
}


void Algorithm::GetSawtoothWave(int& x, int& y, float t, int period, int min, int max)
{
	x = (int)t;
	y = (x % period) * max + min;
}

void Algorithm::GetTriangleWave(int & x, int & y, float t, int period, int min, int max)
{
	x = (int)t;
	y = abs((x % period) - max) + min;
}

void Algorithm::GetSquareWave(int& x, int& y, float t, int period, int max, int min)
{
	x = (int)t;
	y = (x % period) < max ? min : max;
}

//Note that the "period" variable have a different inplication in this case
void Algorithm::GetSineWave(int & x, int & y, float t, int period, int max, int min)
{
	x = (int)t;
	y = (max - min) * (int)sin((float)x / period) + min;
}

void Algorithm::GetSawtoothWave(float & x, float & y, float t, float period, float min, float max)
{
	x = t;
	y = (x - int(x / period) * period) * max + min;
}

void Algorithm::GetTriangleWave(float & x, float & y, float t, float period, float min, float max)
{
	x = t;
	y = abs(x - int(x / period) *  period - max) + min;
}

void Algorithm::GetSquareWave(float & x, float & y, float t, float period, float min, float max)
{
	x = t;
	y = (x - int(x / period) * period) < max ? max : min;
}