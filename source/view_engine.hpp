#pragma once

#include <functional>

typedef int ViewObjectID;

int viewInit();
int viewGetState();

/* 0: monkey warrior; 1: cube; 2: tower; 3: sword; */
ViewObjectID viewAddObject(int type);
int viewRemoveObject(ViewObjectID id);

struct ViewObjectState
{
	float px = 0, py = 0;
	float dx = 0, dy = 0;
};

int viewSetObjectState(ViewObjectID id, const ViewObjectState *state);
int viewGetObjectState(ViewObjectID id, ViewObjectState *state);

/* 2: select; 0: deselect; */
int viewSelectObject(ViewObjectID id, int state);

/* 2 - idle; 0: walk; 1: run; */
int viewSetObjectAnim(ViewObjectID id, int anim, float speed);
int viewGetObjectAnim(ViewObjectID id, float *speed);

int viewSetMouseFunc(std::function<void(int,float,float)> mf);
int viewGetMousePos(float mx, float my, float *wx, float *wy, float *wz);
