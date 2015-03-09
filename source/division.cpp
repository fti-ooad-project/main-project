#include "division.hpp"

#include <functional>

#include <4u/la/mat.hpp>

Division::iterator::iterator(std::list<Member*>::iterator i) :
	iter(i)
{
	
}

Division::iterator::~iterator()
{
	
}

Unit *Division::iterator::operator *()
{
	return (*iter)->unit;
}

Unit *Division::iterator::operator->()
{
	return (*iter)->unit;
}

void Division::iterator::operator ++()
{
	++iter;
}

Division::const_iterator::const_iterator(std::list<Member*>::const_iterator i) :
	iter(i)
{
	
}

Division::const_iterator::~const_iterator()
{
	
}

const Unit *Division::const_iterator::operator *()
{
	return (*iter)->unit;
}

const Unit *Division::const_iterator::operator->()
{
	return (*iter)->unit;
}

void Division::const_iterator::operator ++()
{
	++iter;
}

Division::Division()
{
	
}

Division::~Division()
{
	
}

void Division::addUnit(Unit *u)
{
	Member *m = new Member();
	m->unit = u;
	members.push_back(m);
}

void Division::removeUnit(Unit *u)
{
	members.remove_if([u](Member *m)->bool{
		if(m->unit == u)
		{
			delete m;
			return true;
		}
		return false;
	});
}

Division::iterator Division::begin()
{
	return iterator(members.begin());
}

Division::iterator Division::end()
{
	return iterator(members.end());
}

Division::const_iterator Division::cbegin() const
{
	return const_iterator(members.cbegin());
}

Division::const_iterator Division::cend() const
{
	return const_iterator(members.cend());
}

void Division::updatePositions()
{
	int cnt = 0;
	mat2 rot = mat2(direction.x(),-direction.y(),direction.y(),direction.x());
	int lln = (members.size()/width)*width;
	int llw = members.size()%width;
	vec2 beg = position - distance*rot*vec2(-0.5*(members.size()/width - 1),0.5*(width - 1));
	vec2 llbeg = position - distance*rot*vec2(-0.5*(members.size()/width - 1),0.5*(llw - 1));
	
	for(Member *m : members)
	{
		if(cnt < lln)
		{
			m->unit->setDst(beg + distance*rot*vec2(-cnt/width,cnt%width));
		}
		else
		{
			m->unit->setDst(llbeg + distance*rot*vec2(-cnt/width,cnt%width));
		}
		++cnt;
	}
}

void Division::setPosition(const vec2 &p)
{
	position = p;
}

vec2 Division::getPosition() const
{
	return position;
}

void Division::setDirection(const vec2 &p)
{
	direction = p;
}

vec2 Division::getDirection() const
{
	return direction;
}

void Division::setWidth(int a)
{
	width = a;
}

int Division::getWidth() const
{
	return width;
}

void Division::setDistance(double d)
{
	distance = d;
}

double Division::getDistance() const
{
	return distance;
}

void Division::setSpeed(double s)
{
	speed = s;
}

double Division::getSpeed() const
{
	return speed;
}

void Division::setAngularSpeed(double s)
{
	angular_speed = s;
}

double Division::getAngularSpeed() const
{
	return angular_speed;
}

void Division::setDestination(vec2 d)
{
	destination = d;
}

vec2 Division::getDestination() const
{
	return destination;
}
