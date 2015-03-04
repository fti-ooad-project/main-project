#ifndef DISKRAND_H
#define DISKRAND_H

#include<4u/la/vec.hpp>
#include<4u/rand/contrand.hpp>
#include<4u/util/const.hpp>

#include<math.h>

class DiskRand : public Rand<vec2>
{
private:
	ContRand rand;
public:
	static vec2 wrap(Rand<double> &rand)
	{
		double r = sqrt(rand.get());
		double phi = 2.0*PI*rand.get();
		return r*vec2(cos(phi),sin(phi));
	}
	virtual vec2 get()
	{
		return wrap(rand);
	}
};

#endif // DISKRAND_H
