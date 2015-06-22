#include "view.hpp"
#include "input.hpp"

#include <SDL2/SDL.h>

#include <engine/localsession.hpp>

int main()
{
	const int port = 18765;
	
	LocalSession session = LocalSession(1,port);
	PlayerHandle *handle = session.getPlayerHandle(0);
	DivisionID swordsmen = handle->purchaseDivision(UNIT_SWORDSMAN,0x100);
	DivisionID archers = handle->purchaseDivision(UNIT_ARCHER,0x40);
	
	session.loadMap();
	
	handle->forDivisionHandleID(swordsmen,[](DivisionHandle *division)
	{
		division->setWidth(0x14);
		division->setPosition(vec2(0,-8));
		division->setDistance(0.8);
		division->setDestination(division->getPosition());
	});
	
	handle->forDivisionHandleID(archers,[](DivisionHandle *division)
	{
		division->setWidth(0x8);
		division->setPosition(vec2(0,8));
		division->setDistance(1.2);
		division->setDestination(division->getPosition());
		division->setMode(MODE_FREE);
	});
	
	View view = View(session.getSpectator());
	
	Input input = Input(&view,session.getPlayerHandle(0));
	
	Uint32 tick = SDL_GetTicks();
	while(view.getState() != View::CLOSED)
	{
		Uint32 new_tick = SDL_GetTicks();
		session.process(0.001*(new_tick - tick));
		view.draw();
		tick = new_tick;
	}

	return 0;
}
