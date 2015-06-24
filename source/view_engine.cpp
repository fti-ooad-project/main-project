#include <iostream>
#define RLOG
#include <memory>
#include "view/ViewEngineGL.h"
#include <stdexcept>
#include <openGL/GLincludes.h>

#include "view_engine.hpp"

GraphicSettings ViewManager::_graph_settings;
int GUIProcessor::_focused_elem = -1;

ViewManager *engine = nullptr;
Scene3D *scene = nullptr;

Camera *cam = nullptr;
float phi = PI*0.5f, theta = PI;

std::function<void(int,float,float)> mfunc;

void assertGL( int mark )
{
	if( int i = glGetError() )
	{
#ifdef RLOG
		std::cout << "ogl err at " << mark << ":" << i << "\n";
#endif
		throw std::logic_error( "opengl assert error" );
	}
}
VIEWAPI ViewManager * ViewManager::singletonGet( int type )
{
	static ViewManager* singleton = nullptr;
	if( singleton ) return singleton;
	switch( type )
	{
		case ViewManager::API::GL:
		return singleton = new ViewEngineGL;
		break;
	}
	return nullptr;
}

int viewInit()
{
	engine = ViewManager::singletonGet(ViewManager::API::GL);
	engine->init();
	scene = engine->genScene();
	auto eventer = engine->getEventer();
	
	/*ading lights */
	LightState *ls = scene->getLightStatePtr(scene->genLight());
	ls->_cast_shadow = true;
	ls->_colori = f4( 0.9f , 0.99f , 1.0f , 5.0f );
	ls->_dir = f3( -0.7f , 0.0f , -0.7f );
	ls->_pos = f3( 70.0f , 0.0f , 70.0f );
	ls->_type = LightSourceType::LIGHT_DIRECT;
	
	/*setup camera*/
	cam = scene->getCamera();
	cam->lookAt(f3(0.0f,0.0f,80.0f),f3(0.0f,0.0f,0.0f),f3(0.0f,1.0f,0.0f));
	cam->angle(phi, theta);
	cam->calc();
	
	eventer->setKeyFunc(
		[cam]( const KeyStates &cs , const float dt )
	{
		const float dr = dt*30.0f;
		f3 v( 0.0f , 0.0f , 0.0f );
		
		if( cs.__cur_states[ SDL_SCANCODE_W ] )//w
			v += f3(cos(phi),sin(phi),0.0f);
		if( cs.__cur_states[ SDL_SCANCODE_S ] )//s
			v -= f3(cos(phi),sin(phi),0.0f);
		if( cs.__cur_states[ SDL_SCANCODE_A ] )//a
			v -= f3(sin(phi),-cos(phi),0.0f);
		if( cs.__cur_states[ SDL_SCANCODE_D ] )//d
			v += f3(sin(phi),-cos(phi),0.0f);
		
		cam->_v3pos += v*dr;
		cam->calc();
		
		/*
		ls->_pos = f3( 70.0f , 0.0f , 70.0f );
		ls->_dir = -ls->_pos.g_norm();
		ls->_pos += f3( cam->_v3pos.x() , cam->_v3pos.y() , 0.0f );
		*/
		//cam->$getViewProj().print();
	});
	
	eventer->setMouseFunc([cam,&phi,&theta](const MouseStates &cs, const float dt)
	{
		const float dr = dt * 50.0f;
		
		const float minh = 4.0f;
		const float maxh = 200.0f;
		
		if(cs._mwheel - cs._mwheel_last > 0)
			cam->_v3pos.z() = cam->_v3pos.z()*0.8f;
		else if(cs._mwheel - cs._mwheel_last < 0)
			cam->_v3pos.z() = cam->_v3pos.z()*1.25f;
		
		if(cam->_v3pos.z() < 2.0f*minh)
			cam->_v3pos.z() = 2.0f*minh;
		else if(cam->_v3pos.z() > maxh)
			cam->_v3pos.z() = maxh;
		
		theta = PI - minh/((cam->_v3pos.z() - minh)*0.2f + minh)*PI*0.5f;
		
		if(cs.__cur_states[1])
		{
			phi += dr*(cs.__cur_pos.x() - cs.__last_pos.x());
			/*
			theta += dr*(cs.__cur_pos.y() - cs.__last_pos.y());
			if(theta < 0.5f*PI) theta = 0.5f*PI;
			else if(theta > PI) theta = PI;
			*/
		}
		
		cam->angle(phi, theta);
		cam->calc();
		
		if(mfunc)
		{
			for(int i = 0; i <= 2; ++i)
			{
				if(cs.__cur_states[i] && !cs.__last_states[i])
				{
					mfunc(i,cs.__cur_pos.x(),cs.__cur_pos.y());
				}
			}
		}
		
		engine->getMousePos(-cs.__cur_pos);
	});
	
	/*tell engine to use scene*/
	engine->setScene(scene);
	
	return 0;
}

int viewGetState()
{
	return engine->isInited();
}

ViewObjectID viewAddObject(int type)
{
	int obj_id = scene->genInstance();
	auto *state = scene->getInstanceStatePtr(obj_id);
	state->_view.push_back(type);
	AnimationMixer::TimeEvent obj_event;
	obj_event._active = false;
	state->_animstat.change(2, 0.5f, obj_event);
	state->_animstat.update(0.0f);
	state->_pos = f3(0.0f, 0.0f, 0.0f);
	state->_up = f3(0.0f, 0.0f, 1.0f);
	state->_look = f3(1.0f, 0.0f, 0.0f);
	state->_left = f3(0.0f, -1.0f, 0.0f);
	state->_auto_height = true;
	return obj_id;
}

int viewRemoveObject(ViewObjectID)
{
	return -1;
}

int viewSetObjectState(ViewObjectID id, const ViewObjectState *state)
{
	auto *obj_state = scene->getInstanceStatePtr(id);
	if(obj_state == nullptr)
	{
		return -1;
	}
	obj_state->_pos = f3(state->px,state->py,0.0f);
	obj_state->_look = f3(-state->dx,state->dy,0.0f);
	obj_state->_left = f3(state->dy,state->dx,0.0f);
	return 0;
}

int viewGetObjectState(ViewObjectID id, ViewObjectState *state)
{
	auto *obj_state = scene->getInstanceStatePtr(id);
	if(obj_state == nullptr)
	{
		return -1;
	}
	state->px = obj_state->_pos.x();
	state->py = obj_state->_pos.y();
	state->dx = obj_state->_look.x();
	state->dy = obj_state->_look.y();
	return 0;
}

int viewSelectObject(ViewObjectID id, int state)
{
	auto *obj_state = scene->getInstanceStatePtr(id);
	if(obj_state == nullptr)
	{
		return -1;
	}
	obj_state->selectid = state;
}

int viewSetObjectAnim(ViewObjectID id, int anim, float speed)
{
	auto *obj_state = scene->getInstanceStatePtr(id);
	if(obj_state == nullptr)
	{
		return -1;
	}
	if(obj_state->_animstat._moment._cur_set != anim)
	{
		AnimationMixer::TimeEvent obj_event;
		obj_event._active = false;
		obj_state->_animstat.change(anim, speed, obj_event);
	}
	else
	{
		obj_state->_animstat._speed = speed;
	}
}

int viewGetObjectAnim(ViewObjectID id, float *speed)
{
	auto *obj_state = scene->getInstanceStatePtr(id);
	if(obj_state == nullptr)
	{
		return -1;
	}
	*speed = obj_state->_animstat._speed;
	return obj_state->_animstat._moment._cur_set;
}

int viewSetMouseFunc(std::function<void(int index, float px, float py)> mf)
{
	mfunc = mf;
	return 0;
}

int viewGetMousePos(float mx, float my, float *wx, float *wy, float *wz)
{
	f3 w = engine->getMousePos(-f2(mx,my));
	*wx = w.x();
	*wy = w.y();
	*wz = w.z();
	return 0;
}
