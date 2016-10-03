#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <cmath>
#include <DirectXMath.h>
namespace Math {
	static const float MATH_PI = 3.14159265358979323846f;
}
namespace Algorithm {
	int GetParabola(int x);
	//Stores the result in x and y
	void GetParabola(int& x, int& y, float t);
	//Stores the result in x and y
	void GetCircle(int& x, int&y, float t);
	//
	void GetEllipse(int& x, int& y, float t, int a, int b);
	//xLobes and yLobes describe the number of lobes of the figure
	void GetLissajousCurve(int& x, int& y, float t, int a, int b, int xLobes, int yLobes);
	//A curve traced by a point attached to a circle of radius r rolling around the inside of a fixed circle of radius R, where the point is at a distance d from the center of the interior circle.
	void GetHypotrochoid(int& x, int&y, float t, int bigR, int r, int d);

	//Stores the result in x and y
	void GetParabola(float& x, float& y, float t);
	//Stores the result in x and y
	void GetCircle(float& x, float&y, float t);
	//
	void GetEllipse(float& x, float& y, float t, float a, float b);
	//xLobes and yLobes describe the number of lobes of the figure
	void GetLissajousCurve(float& x, float& y, float t, float a, float b, float xLobes, float yLobes);
	//A curve traced by a pofloat attached to a circle of radius r rolling around the inside of a fixed circle of radius R, where the pofloat is at a distance d from the center of the floaterior circle.
	void GetHypotrochoid(float& x, float&y, float t, float bigR, float r, float d);

	void GetSawtoothWave(int& x, int& y, float t, int period, int min, int max);
	void GetTriangleWave(int & x, int & y, float t, int period, int min, int max);
	void GetSquareWave(int & x, int & y, float t, int period, int min, int max);
	void GetSineWave(int& x, int& y, float t, int period, int min, int max);
	void GetSawtoothWave(float& x, float& y, float t, float period, float min, float max);
	void GetTriangleWave(float & x, float & y, float t, float period, float min, float max);
	void GetSquareWave(float & x, float & y, float t, float period, float min, float max);
}


#endif
