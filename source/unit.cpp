#include "unit.hpp"

Unit::Unit()
{
    
}

Unit::~Unit()
{
    
}

vec2 Unit::getDir() const
{
	return dir;
}

void Unit::setDir(const vec2 &d)
{
	dir = d;
}

vec2 Unit::getDst() const
{
	return dst;
}

void Unit::setDst(const vec2 &d)
{
	dst = d;
}

double Unit::getSpd() const
{
	return spd;
}

void Unit::setSpd(double s)
{
	spd = s;
}
