#include <list>

#include <media/media.h>
#include <graphics/graphics.h>

#include <engine-source/unit.hpp>
#include <engine-source/division.hpp>
#include <engine-source/processor.hpp>
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

void handleAppEvent(const Media_AppEvent *event, void *data)
{
	State *state = static_cast<State*>(data);
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

void handleSurfaceEvent(const Media_SurfaceEvent *event, void *data)
{
	State *state = static_cast<State*>(data);
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
}

void handleMotionEvent(const Media_MotionEvent *event, void *data)
{
	State *state = static_cast<State*>(data);
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
			state->division.updatePositions();
		}
		break;
	case MEDIA_ACTION_MOVE:
		// printInfo("Move(index: %d, pos: (%d,%d))\n",event->index,event->x,event->y);
		break;
	case MEDIA_ACTION_WHEEL:
		if(event->yval > 0)
		{
			state->view.factor *= 1.2;
		}
		else
		if(event->yval < 0)
		{
			state->view.factor /= 1.2;
		}
		break;
	default:
		break;
	}
	//printInfo("Motion ( %d, %d )\n", static_cast<int>(pos.x()), static_cast<int>(pos.y()));
}

void handleSensorEvent(const Media_SensorEvent *event, void *data)
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
	
	gClear();
	
	gSetColorInt(G_RED);
	for(Unit *u : units)
	{
		state->view.drawUnit(u);
		state->view.drawUnitDst(u);
	}
	
	gSetColorInt(G_YELLOW);
	for(Object *o : objects)
	{
		state->view.drawObject(o);
	}
}

int Media_main(Media_App *app)
{
	State state = {0,0,0,0,false};
	state.division.setWidth(0x14);
	state.division.setPosition(vec2(0,0));
	state.division.setDistance(0.8);
	state.division.setDestination(state.division.getPosition());
	state.division.setDirection(vec2(0,1));
	state.division.setSpeed(0.5);
	
	int units_num = 0x100;
	int rows_num = sqrt(units_num);
	for(int i = 0; i < units_num; ++i)
	{
		Unit *u;
		u = new Unit();
		u->setInvMass(1.0/80.0);
		u->setSize(0.25);
		u->setPos(vec2(rows_num/2 - i/rows_num,rows_num/2 - i%rows_num));
		u->setSpd(1.0);
		state.units.push_back(u);
		state.division.addUnit(u);
		state.proc.addObject(u);
		state.proc.addUnit(u);
	}
	
	for(int i = 0; i < 0xa; ++i)
	{
		Object *o;
		o = new Object();
		o->setInvMass(0.0001);
		o->setSize(2.0);
		o->setPos(5.0*vec2(i%5 - 2,i/5));
		state.objects.push_back(o);
		state.proc.addObject(o);
	}
	
	state.proc.addDivision(&state.division);
	
	state.division.redistribute();
	state.division.updatePositions();
	
	app->data = static_cast<void*>(&state);
	
	Media_Listener listener;
	listener.app = &handleAppEvent;
	listener.surface = &handleSurfaceEvent;
	listener.motion = &handleMotionEvent;
	listener.sensor = &handleSensorEvent;
	listener.data = static_cast<void*>(&state);
	Media_addListener(app,&listener);

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
			int iter = 0x1;
			for(int i = 0; i < iter; ++i)
			{
				state.proc.attract();
				state.proc.move(0.04/iter);
				state.proc.interact();
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
