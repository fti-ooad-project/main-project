#include "division.hpp"

#include <algorithm>
#include <functional>
#include <vector>
#include <map>

#include <cassert>

#include <4u/la/mat.hpp>

#include <media/media.h>

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

vec2 Division::getRelPos(int row, int col) const
{
	int rows = (members.size() - 1)/width + 1;
	int lcols = members.size()%width;
	if(row < int(members.size())/width)
	{
		return distance*vec2(0.5*(rows - 1) - row, col - 0.5*(width - 1));
	}
	else
	{
		return distance*vec2(0.5*(rows - 1) - row, col - 0.5*(lcols - 1));
	}
}

void Division::updatePositions()
{
	mat2 rot = mat2(direction.x(),-direction.y(),direction.y(),direction.x());
	
	for(Member *m : members)
	{
		m->unit->setDst(position + rot*getRelPos(m->row,m->col));
	}
}

void Division::movePositions(const vec2 &dp)
{
	for(Member *m : members)
	{
		m->unit->setDst(m->unit->getDst() + dp);
	}
}

void Division::redistribute()
{
	int size = members.size();
	Place *ps = new Place[size];
	MemberDist *ms = new MemberDist[size];
	
	mat2 rot = mat2(direction.x(),-direction.y(),direction.y(),direction.x());
	for(int i = 0; i < size; ++i)
	{
		ps[i].row = i/width;
		ps[i].col = i%width;
		ps[i].pos = position + rot*getRelPos(ps[i].row,ps[i].col);
	}
	
	int cnt = 0;
	for(Member *m : members)
	{
		ms[cnt].m = m;
		double md = INFINITY;
		for(int j = 0; j < size; ++j)
		{
			double d = sqr(ps[j].pos - m->unit->getPos());
			if(d < md)
			{
				md = d;
			}
		}
		ms[cnt].min_dist = sqrt(md);
		++cnt;
	}
	
	std::sort(ms,ms+size,[](MemberDist a, MemberDist b)->bool{return b.min_dist < a.min_dist;});
	
	for(int i = 0; i < size; ++i)
	{
		Place *p = nullptr;
		double md = INFINITY;
		for(int j = 0; j < size; ++j)
		{
			if(ps[j].m)
			{
				continue;
			}
			double d = sqr(ps[j].pos - ms[i].m->unit->getPos());
			if(d < md)
			{
				p = &ps[j];
				md = d;
			}
		}
		if(p == nullptr)
		{
			break;
		}
		
		p->m = &ms[i];
		ms[i].dist = sqrt(md);
	}
	
	int shakes = size;
	for(int k = 0; k < shakes; ++k)
	{
		Place *p = nullptr;
		double mdd = 0.0;
		for(int i = 0; i < size; ++i)
		{
			double dd = ps[i].m->dist - ps[i].m->min_dist;
			if(dd > mdd)
			{
				mdd = dd;
				p = &ps[i];
			}
		}
		if(p == nullptr)
		{
			break;
		}
		
		Place *pd = nullptr;
		mdd = INFINITY;
		for(int i = 0; i < size; ++i)
		{
			double dd = length(p->m->m->unit->getPos() - ps[i].pos) + length(ps[i].m->m->unit->getPos() - p->pos);
			if(dd < mdd)
			{
				mdd = dd;
				pd = &ps[i];
			}
		}
		if(pd == nullptr)
		{
			break;
		}
		
		MemberDist *m = p->m;
		p->m = pd->m;
		pd->m = m;
		
		p->m->dist = length(p->m->m->unit->getPos() - p->pos);
		pd->m->dist = length(pd->m->m->unit->getPos() - pd->pos);
	}
	
	for(int i = 0; i < size; ++i)
	{
		ps[i].m->m->col = ps[i].col;
		ps[i].m->m->row = ps[i].row;
	}
	
	delete ps;
	delete ms;
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

void Division::setDestination(vec2 d)
{
	destination = d;
}

vec2 Division::getDestination() const
{
	return destination;
}
