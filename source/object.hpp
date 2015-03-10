#pragma once

#include <4u/la/vec.hpp>

class Object
{
private:
	double inv_mass = 0.0;
	vec2 pos = nullvec2;
	vec2 vel = nullvec2;
	double size = 1.0;
	
public:
	Object();
	virtual ~Object();
	
	double getInvMass() const;
	void setInvMass(double im);
	
	vec2 getPos() const;
	void setPos(const vec2 &p);
	
	vec2 getVel() const;
	void setVel(const vec2 &v);
	
	double getSize() const;
	void setSize(double s);
};
