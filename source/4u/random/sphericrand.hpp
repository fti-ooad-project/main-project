#ifndef SPHERICRAND_HPP
#define SPHERICRAND_HPP

#include"rand.hpp"
#include"contrand.hpp"

#include<cmath>
#include<4u/util/const.hpp>
#include<4u/la/vec.hpp>

#include<4u/util/const.hpp>

class SphericRand : public Rand<vec3>
{
private:
	ContRand generator;
public:
	static vec3 wrap(Rand<double> &rand)
	{
		double phi = 2.0*PI*rand.get();
		double theta = acos(1.0 - 2.0*rand.get());
		return vec3(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
	}
	virtual vec3 get()
	{
		return wrap(generator);
	}
};

class SemiSphericRand : public Rand<vec3, const vec3 &>
{
private:
	SphericRand generator;
	static vec3 reflect(const vec3 &rand, const vec3 &normal)
	{
		double proj = rand*normal;
		return rand - static_cast<double>(proj < 0)*(2.0*normal*proj);
	}
public:
	static vec3 wrap(Rand<vec3> &spheric_rand, const vec3 &normal)
	{
		return reflect(spheric_rand.get(),normal);
	}
	static vec3 wrap(Rand<double> &rand, const vec3 &normal)
	{
		return reflect(SphericRand::wrap(rand),normal);
	}
	virtual vec3 get(const vec3 &normal)
	{
		return wrap(generator,normal);
	}
};

class SemiSphericCosineRand : public Rand<vec3, const vec3 &>
{
private:
	ContRand generator;

public:
	static vec3 wrap(ContRand &rand, const vec3 &normal)
	{
		// Generates basis for <normal,*> kernel
		vec3 nx, ny;
		if(vec3(0,0,1)*normal < 0.6 && vec3(0,0,1)*normal > -0.6)
		{
			nx = vec3(0,0,1);
		}
		else
		{
			nx = vec3(1,0,0);
		}
		ny = norm(nx^normal);
		nx = ny^normal;

		// Computes distribution
		double phi = 2.0*PI*rand.get();
		double theta = acos(1.0 - 2.0*rand.get())/2.0;
		return nx*cos(phi)*sin(theta) + ny*sin(phi)*sin(theta) + normal*cos(theta);
	}
	virtual vec3 get(const vec3 &normal)
	{
		return wrap(generator,normal);
	}
};

#endif // SPHERICRAND_HPP
