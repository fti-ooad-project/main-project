#include <list>

#include <media/media.h>
#include <graphics/graphics.h>

#include "unit.hpp"
#include "division.hpp"
#include "processor.hpp"
#include "view.hpp"

struct State
{
	int done;
	int wait;
	int width;
	int height;
	bool ready;
	
	Processor proc;
	std::list<Object*> objects;
	std::list<Unit*> units;
	Division division;
	
	View view;
};

void handleAppEvent(Media_App *app, const Media_AppEvent *event)
{
	State *state = static_cast<State*>(app->data);
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

void handleSurfaceEvent(Media_App *app, const Media_SurfaceEvent *event)
{
	State *state = static_cast<State*>(app->data);
	switch(event->type)
	{
	case MEDIA_SURFACE_INIT:
		printInfo("Init surface\n");
		initGraphics();
		break;
	case MEDIA_SURFACE_TERM:
		printInfo("Term surface\n");
		disposeGraphics();
		break;
	case MEDIA_SURFACE_RESIZE:
		printInfo("Resize surface ( %d, %d )\n",event->w,event->h);
		state->width = event->w;
		state->height = event->h;
		resizeGraphics(event->w,event->h);
		state->ready = true;
		break;
	default:
		break;
	}
}

void handleMotionEvent(Media_App *app, const Media_MotionEvent *event)
{
	State *state = static_cast<State*>(app->data);
	switch(event->action)
	{
	case MEDIA_ACTION_UP:
		//printInfo("Up\n");
		break;
	case MEDIA_ACTION_DOWN:
		printInfo("Down(index: %d, button: %d, pos: (%d,%d))\n",event->index,event->button,event->x,event->y);
		if(event->button == MEDIA_BUTTON_LEFT)
		{
			vec2 spos = vec2(event->x,event->y);
			state->division.setDestination(state->view.posStoW(spos));
			state->division.setDirection(norm(state->division.getDestination() - state->division.getPosition()));
			state->division.redistribute();
		}
		break;
	case MEDIA_ACTION_MOVE:
		// printInfo("Move(index: %d, pos: (%d,%d))\n",event->index,event->x,event->y);
		break;
	case MEDIA_ACTION_WHEEL_UP:
		state->view.factor *= 1.2;
		break;
	case MEDIA_ACTION_WHEEL_DOWN:
		state->view.factor *= 1.0/1.2;
		break;
	default:
		break;
	}
	//printInfo("Motion ( %d, %d )\n", static_cast<int>(pos.x()), static_cast<int>(pos.y()));
}

void handleSensorEvent(Media_App *app, const Media_SensorEvent *event)
{
	switch(event->sensor)
	{
	case MEDIA_SENSOR_ACCELEROMETER:
		// printInfo("Accelerometer ( %f, %f, %f)\n",event->x,event->y,event->z);
		break;
	default:
		break;
	}
}

void render(Media_App *app)
{
	State *state = static_cast<State*>(app->data);
	auto &units = state->units;
	auto &objects = state->objects;
	
	clear();
	
	setColorInt(RED);
	for(Unit *u : units)
	{
		state->view.drawUnit(u);
		state->view.drawUnitDst(u);
	}
	
	setColorInt(YELLOW);
	for(Object *o : objects)
	{
		state->view.drawObject(o);
	}
}

int Media_main(Media_App *app)
{
	State state = {0,0,0,0,false};
	state.division.setWidth(0x6);
	state.division.setPosition(vec2(0,-2));
	state.division.setDistance(0.8);
	state.division.setDestination(state.division.getPosition());
	state.division.setDirection(vec2(0,1));
	state.division.setSpeed(0.5);
	
	for(int i = 0; i < 0x10; ++i)
	{
		Unit *u;
		u = new Unit();
		u->setInvMass(1.0/80.0);
		u->setSize(0.25);
		u->setPos(vec2(8-i,-4));
		u->setSpd(1.0);
		state.units.push_back(u);
		state.division.addUnit(u);
		state.proc.addObject(u);
		state.proc.addUnit(u);
	}
	
	for(int i = 0; i < 0x10; ++i)
	{
		Object *o;
		o = new Object();
		o->setInvMass(0.0005);
		o->setSize(0.5);
		o->setPos(vec2(i%8 - 3.5,i/8));
		state.objects.push_back(o);
		state.proc.addObject(o);
	}
	
	state.proc.addDivision(&state.division);
	
	state.division.redistribute();
	state.division.updatePositions();
	
	app->data = static_cast<void*>(&state);

	app->listeners.app = &handleAppEvent;
	app->listeners.surface = &handleSurfaceEvent;
	app->listeners.motion = &handleMotionEvent;
	app->listeners.sensor = &handleSensorEvent;

	app->renderer = &render;
	
	for(;;)
	{
		if(state.wait)
		{
			Media_waitForEvent(app);
		}
		Media_handleEvents(app);

		if(state.done)
		{
			break;
		}

		if(state.ready)
		{
			for(int i = 0; i < 0x10; ++i)
			{
				state.proc.attract();
				state.proc.move(0.0025);
				state.proc.interact(0.1);
			}
			state.division.updatePositions();
		}

		// printInfo("Frame\n");
		Media_renderFrame(app);
	}
	
	for(Unit *u : state.units)
	{
		delete u;
	}

	return 0;
}
