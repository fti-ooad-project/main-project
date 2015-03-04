#pragma once

#include <list>
#include <utility>

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
	int width;
	std::list<Member*> members;
	
public:
	Division();
	virtual ~Division();
	
	void addUnit(Unit *u);
	void removeUnit(Unit *u);
	
	iterator begin();
	iterator end();
	
	const_iterator cbegin() const;
	const_iterator cend() const;
	
	void setWidth(int a);
	int getWidth() const;
};
