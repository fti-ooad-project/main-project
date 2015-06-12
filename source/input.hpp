#pragma once

#include <4u/la/vec.hpp>
#include <4u/la/mat.hpp>

#include <media/media.h>
#include <graphics/graphics.h>
#include <gui/factory.hpp>
#include <gui/handler.hpp>

#include "view.hpp"
#include <engine/playerhandle.hpp>
#include <engine/divisionhandle.hpp>

class Input
{
private:
	
	int done = 0;
	int wait = 0;
	int width = 0;
	int height = 0;
	bool ready = 0;
	
	Media_App *app;
	View *view;
	PlayerHandle *player;
	DivisionID division = 0;
	
	Media_Listener listener;
	
	gui::Factory factory;
	gui::MediaHandler handler;
	gui::Screen *screen = nullptr;
	gui::ToggleButton *new_div = nullptr;
	
	static void handleAppEvent(const Media_AppEvent *event, void *data)
	{
		Input *state = static_cast<Input*>(data);
		switch(event->type)
		{
		case MEDIA_APP_SAVESTATE:
			printInfo("Save state\n");
			break;
		case MEDIA_APP_QUIT:
			state->done = 1;
			printInfo("Quit\n");
			break;
		case MEDIA_APP_SHOW:
			state->wait = 0;
			printInfo("Show\n");
			break;
		case MEDIA_APP_HIDE:
			state->wait = 1;
			printInfo("Hide\n");
			break;
		default:
			break;
		}
	}
	
	static void handleSurfaceEvent(const Media_SurfaceEvent *event, void *data)
	{
		Input *state = static_cast<Input*>(data);
		switch(event->type)
		{
		case MEDIA_SURFACE_INIT:
			printInfo("Init surface\n");
			gInit();
			break;
		case MEDIA_SURFACE_TERM:
			printInfo("Term surface\n");
			gDispose();
			break;
		case MEDIA_SURFACE_RESIZE:
			printInfo("Resize surface ( %d, %d )\n",event->w,event->h);
			state->width = event->w;
			state->height = event->h;
			gResize(event->w,event->h);
			state->ready = true;
			break;
		default:
			break;
		}
		state->handler.handleSurface(event);
	}
	
	static void handleMotionEvent(const Media_MotionEvent *event, void *data)
	{
		Input *state = static_cast<Input*>(data);
		if(!state->handler.handleMotion(event))
		{
			switch(event->action)
			{
			case MEDIA_ACTION_UP:
				//printInfo("Up\n");
				break;
			case MEDIA_ACTION_DOWN:
				printInfo("Down(index: %d, button: %d, pos: (%d,%d))\n",event->index,event->button,event->x,event->y);
				if(event->button == MEDIA_BUTTON_RIGHT)
				{
					state->player->forDivisionHandleID(state->division,[&state,event](DivisionHandle *division)
					{
						vec2 spos = vec2(event->x,event->y);
						division->setDestination(state->view->posStoW(spos));
						division->setDirection(norm(division->getDestination() - division->getPosition()));
					});
				}
				else
				if(event->button == MEDIA_BUTTON_LEFT)
				{
					state->player->forEachDivisionHandle([&state,event](DivisionHandle *division)
					{
						vec2 wpos = state->view->posStoW(vec2(event->x,event->y));
						vec2 dir = division->getDirection();
						mat2 rot = {dir.y(),-dir.x(),dir.x(),dir.y()};
						vec2 drp = rot*(wpos - division->getPosition())/division->getDistance();
						drp.x() /= division->getWidth();
						drp.y() /= (division->getUnitsCount()/division->getWidth() + 1);
						if(fabs(drp.x()) < 0.5 && fabs(drp.y()) < 0.5)
						{
							state->division = division->getID();
							state->view->setSelection(division->getID());
						}
					});
				}
				break;
			case MEDIA_ACTION_MOVE:
				// printInfo("Move(index: %d, pos: (%d,%d))\n",event->index,event->x,event->y);
				break;
			case MEDIA_ACTION_WHEEL:
				if(event->yval > 0)
				{
					state->view->factor *= 1.2;
				}
				else
				if(event->yval < 0)
				{
					state->view->factor /= 1.2;
				}
				break;
			default:
				break;
			}
		}
		//printInfo("Motion ( %d, %d )\n", static_cast<int>(pos.x()), static_cast<int>(pos.y()));
	}
	
public:
	Input(Media_App *a, View *v, PlayerHandle *s)
	  : app(a), view(v), player(s), factory(a)
	{
		listener.app = &handleAppEvent;
		listener.surface = &handleSurfaceEvent;
		listener.motion = &handleMotionEvent;
		listener.sensor = nullptr;
		listener.data = static_cast<void*>(this);
		Media_addListener(app,&listener);
		
		initUI();
	}

	~Input()
	{
		
	}
	
	bool handle()
	{
		if(wait)
		{
			Media_waitForEvent(app);
		}
		Media_handleEvents(app);
		handler.updateState();

		return !done;
	}
	
	void initUI()
	{
		handler.setMediaApp(app);
		
		screen = factory.produceScreen();
		handler.setScreen(screen);
		
		new_div = factory.produceToggleButton();
		new_div->setRelativeBounds(vec2(0.12,0.06));
		new_div->setRelativePosition(vec2(0.86,0.92));
		new_div->setText(L"+");
		new_div->setColor(vec4(0,0,0.6,1));
		new_div->setToggleColor(vec4(0,0.8,0,1));
		new_div->setTextSize(32);
		new_div->setTextColor(vec4(1,1,1,1));
		screen->addChild(new_div);
	}
	
	void drawUI()
	{
		screen->draw();
	}
	
	void setDivisionID(DivisionID id)
	{
		division = id;
	}
};
