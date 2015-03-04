#include "division.hpp"

#include <functional>

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
	u->setDst(vec2(members.size()%width,members.size()/width));
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

void Division::setWidth(int a)
{
	width = a;
}

int Division::getWidth() const
{
	return width;
}
