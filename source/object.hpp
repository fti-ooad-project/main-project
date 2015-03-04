#pragma once

#include <4u/la/vec.hpp>

class Object
{
private:
	double inv_mass;
	vec2 pos;
	vec2 vel;
	double size;
	
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
