#pragma once

#include "object.hpp"

#include<4u/la/vec.hpp>

class Unit : public Object
{
private:
	vec2 dir;
	vec2 dst;
	
	double spd;
	bool alive;

public:
	Unit();
	virtual ~Unit();
	
	vec2 getDir() const;
	void setDir(const vec2 &d);
	
	vec2 getDst() const;
	void setDst(const vec2 &d);
	
	double getSpd() const;
	void setSpd(double s);
};
