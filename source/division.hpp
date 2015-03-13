#pragma once

#include <list>
#include <utility>

#include <4u/la/vec.hpp>

#include "unit.hpp"

class Division
{
private:
	class Member
	{
	public:
		Unit *unit;
		vec2 rel_pos;
		int row, col;
	};
	
public:
	class iterator
	{
	private:
		std::list<Member*>::iterator iter;
	public:
		iterator(std::list<Member*>::iterator i);
		~iterator();
		Unit *operator *();
		Unit *operator->();
		void operator ++();
	};
	
	class const_iterator
	{
	private:
		std::list<Member*>::const_iterator iter;
	public:
		const_iterator(std::list<Member*>::const_iterator i);
		~const_iterator();
		const Unit *operator *();
		const Unit *operator->();
		void operator++();
	};
	
private:
	vec2 position = nullvec2;
	vec2 direction = vec2(0,1);
	
	int width = 8;
	double distance = 1.0;
	
	double speed = 1.0;
	vec2 destination = nullvec2;
	
	std::list<Member*> members;
	
	vec2 getRelPos(int row, int col) const;
	
	struct MemberDist
	{
		Member *m = nullptr;
		double min_dist = 0.0;
		double dist = 0.0;
	};
	
	struct Place
	{
		int row, col;
		vec2 pos;
		MemberDist *m = nullptr;
	};
	
public:
	Division();
	virtual ~Division();
	
	void addUnit(Unit *u);
	void removeUnit(Unit *u);
	
	iterator begin();
	iterator end();
	
	const_iterator cbegin() const;
	const_iterator cend() const;
	
	void updatePositions();
	void movePositions(const vec2 &dp);
	void redistribute();
	
	void setPosition(const vec2 &p);
	vec2 getPosition() const;
	
	void setDirection(const vec2 &p);
	vec2 getDirection() const;
	
	void setWidth(int a);
	int getWidth() const;
	
	void setDistance(double a);
	double getDistance() const;
	
	void setSpeed(double s);
	double getSpeed() const;
	
	void setDestination(vec2 d);
	vec2 getDestination() const;
};
