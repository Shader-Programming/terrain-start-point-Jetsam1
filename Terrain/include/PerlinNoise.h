#pragma once

#include <vector>

class Perlin
{
private:
	double fade(double t);
	double Lerp(double t, double a, double b);
	double Grad(int hash, double x, double y, double z);
	std::vector<int> p;

public:
	Perlin();
	Perlin(unsigned int seed); //takes a seed in
	double noise(double x, double y, double z);//makes noise
};
