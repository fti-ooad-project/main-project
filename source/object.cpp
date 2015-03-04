#include "object.hpp"

Object::Object()
{
	
}

Object::~Object()
{
	
}

double Object::getInvMass() const
{
	return inv_mass;
}

void Object::setInvMass(double im)
{
	inv_mass = im;
}

vec2 Object::getPos() const
{
    return pos;
}

void Object::setPos(const vec2 &p)
{
    pos = p;
}

vec2 Object::getVel() const
{
    return vel;
}

void Object::setVel(const vec2 &v)
{
    vel = v;
}

double Object::getSize() const
{
	return size;
}

void Object::setSize(double s)
{
	size = s;
}
