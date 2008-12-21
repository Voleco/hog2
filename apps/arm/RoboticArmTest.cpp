/*
 *  RoboticArmTest.cpp
 *  hog2
 *
 *  Created by Nathan Sturtevant on 11/15/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 * This file is part of HOG.
 *
 * HOG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * HOG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with HOG; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "Common.h"
#include "UnitSimulation.h"
#include "EpisodicSimulation.h"
#include <deque>
#include "IDAStar.h"
#include "TemplateAStar.h"
#include "RoboticArmTest.h"

#include "RoboticArm.h"

const int numArms = 3;
RoboticArm *r = 0;
armAngles config;
armAngles goal;
bool validSearch = false;
unsigned int pathLoc = 0;
std::vector<armAngles> ourPath;
TemplateAStar<armAngles, armRotations, RoboticArm> astar;
float totalTime;

bool mouseTracking;
int px1, py1, px2, py2;
int absType = 0;

//std::vector<PuzzleSimulation *> unitSims;

int main(int argc, char* argv[])
{
	InstallHandlers();
	setlinebuf( stdout );
	RunHOGGUI(argc, argv);
}


/**
 * This function is used to allocate the unit simulated that you want to run.
 * Any parameters or other experimental setup can be done at this time.
 */
void CreateSimulation(int)
{
	if (r != 0)
		delete r;
	r = new RoboticArm(numArms, 1.0/(double)numArms);
//	r->AddObstacle(line2d(recVec(-1,  1, 0), recVec( 1,  1, 0)));
//	r->AddObstacle(line2d(recVec(-1, -1, 0), recVec( 1, -1, 0)));
//	r->AddObstacle(line2d(recVec( 1, -1, 0), recVec( 1,  1, 0)));
//	r->AddObstacle(line2d(recVec(-1, -1, 0), recVec(-1,  1, 0)));
	r->AddObstacle(line2d(recVec(-0.30, -0.30, 0), recVec(0.30, -0.30, 0)));
	r->AddObstacle(line2d(recVec(-0.30, -0.28, 0), recVec(-0.30, -0.32, 0)));
	r->AddObstacle(line2d(recVec(0.30, 0.30, 0), recVec(0.30, -0.30, 0)));
	r->AddObstacle(line2d(recVec(0.28, 0.30, 0), recVec(0.32, 0.30, 0)));
	config.SetNumArms(numArms);
	do {
	  for (int x = 0; x < numArms; x++)
	    config.SetAngle(x, 512+random()%512);
	} while( !r->LegalState( config ) );
//	unitSims.resize(id+1);
//	unitSims[id] = new PuzzleSimulation(new MNPuzzle(4, 4));
//	unitSims[id]->SetStepType(kMinTime);


	srandom( 7 );

#if 0
	// some randomly generated goals
	goal.SetGoal( 0.256288, -0.2005733 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
	goal.SetGoal( -0.344304, 0.453280 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
	goal.SetGoal( -0.119685, -0.437794 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
	goal.SetGoal( -0.651559, 0.052971 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
	goal.SetGoal( 0.608928, 0.593450 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
#else
#if 0
	if( r->GenerateMaxDistHeuristics( config, 5 ) < 5 ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
#else
	// goals generated by GenerateMaxDistHeuristics
	goal.SetGoal( -0.146228, -0.299603 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
	goal.SetGoal( 0.153535, -0.299558 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
	goal.SetGoal( 0.301519, 0.205166 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
	goal.SetGoal( 0.017949, -0.036776 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
	goal.SetGoal( -0.329444, 0.009723 );
	if( !r->GenerateHeuristic( config, goal ) ) {
	  printf( "WARNING: INVALID GOAL FOR HEURISTIC\n" );
	}
#endif
#endif

	for( int i = 0; i < 1000; ++i ) {
	  double x, y;

	  do {
	    for( int s = 0; s < numArms; s++ ) {
	      config.SetAngle( s, random() & 0x3FE );
	    }
	  } while( !r->LegalState( config ) );

	  
	  do {

	    x = (double)random() / (double)RAND_MAX * 2.0 - 1.0;
	    y = (double)random() / (double)RAND_MAX * 2.0 - 1.0;
	  } while( !r->ValidGoalPosition( x, y ) );

	  goal.SetGoal( x, y );

	  std::cout << "Starting search between: " << config << " and " << goal << std::endl;

	  Timer t;
	  t.startTimer();
	  astar.GetPath( r, config, goal, ourPath );
	  if( ourPath.size() > 0 ) {
	    printf( "Done!" );
	  } else {
	    printf( "No solution!" );
	  }
	  printf( " %d nodes expanded; %1.1f nodes/sec\n",
		  astar.GetNodesExpanded(),
		  (double)astar.GetNodesExpanded() / t.endTimer() );
	}
}

/**
 * Allows you to install any keyboard handlers needed for program interaction.
 */
void InstallHandlers()
{
	//InstallKeyboardHandler(MyDisplayHandler, "Toggle Abstraction", "Toggle display of the ith level of the abstraction", kAnyModifier, '0', '9');
	InstallKeyboardHandler(MyDisplayHandler, "Cycle Abs. Display", "Cycle which group abstraction is drawn", kAnyModifier, '\t');
	InstallKeyboardHandler(MyDisplayHandler, "Pause Simulation", "Pause simulation execution.", kNoModifier, 'p');
	InstallKeyboardHandler(MyDisplayHandler, "Step Simulation", "If the simulation is paused, step forward .1 sec.", kNoModifier, 'o');
	InstallKeyboardHandler(MyDisplayHandler, "Step History", "If the simulation is paused, step forward .1 sec in history", kAnyModifier, '}');
	InstallKeyboardHandler(MyDisplayHandler, "Step History", "If the simulation is paused, step back .1 sec in history", kAnyModifier, '{');
	InstallKeyboardHandler(MyDisplayHandler, "Step Abs Type", "Increase abstraction type", kAnyModifier, ']');
	InstallKeyboardHandler(MyDisplayHandler, "Step Abs Type", "Decrease abstraction type", kAnyModifier, '[');
	
	InstallKeyboardHandler(MyKeyHandler, "0-9", "select segment", kNoModifier, '0', '9');
	InstallKeyboardHandler(MyKeyHandler, "Rotate segment", "rotate segment CW", kNoModifier, 'a');
	InstallKeyboardHandler(MyKeyHandler, "Rotate segment", "rotate segment CCW", kNoModifier, 's');
	
	InstallCommandLineHandler(MyCLHandler, "-map", "-map filename", "Selects the default map to be loaded.");
	
	InstallWindowHandler(MyWindowHandler);
	
	InstallMouseClickHandler(MyClickHandler);
}

void MyWindowHandler(unsigned long windowID, tWindowEventType eType)
{
	if (eType == kWindowDestroyed)
	{
		printf("Window %ld destroyed\n", windowID);
		RemoveFrameHandler(MyFrameHandler, windowID, 0);
	}
	else if (eType == kWindowCreated)
	{
		printf("Window %ld created\n", windowID);
		InstallFrameHandler(MyFrameHandler, windowID, 0);
		CreateSimulation(windowID);
		SetNumPorts(windowID, 1);
	}
}

void MyFrameHandler(unsigned long windowID, unsigned int /*viewport*/, void *)
{
	static int currFrame = 0;
	currFrame++;

	r->OpenGLDraw(windowID);
	if (!validSearch)
	{
		if (ourPath.size() == 0)
			r->OpenGLDraw(windowID, config);
		else
			r->OpenGLDraw(windowID, ourPath[(pathLoc++)%ourPath.size()]);
	}
	else {
		Timer t;
		t.startTimer();
		for (int x = 0; x < 1000; x++)
		{
			if (astar.DoSingleSearchStep(ourPath))
			{
				validSearch = false;
				if (ourPath.size() > 0)
				{
					totalTime += t.endTimer();
					printf("Done! %d nodes expanded; %1.1f nodes/sec\n",
						   astar.GetNodesExpanded(), (double)astar.GetNodesExpanded()/totalTime);
					config = ourPath.back();
					pathLoc = 0;
					break;
				}
			}
		}
		totalTime += t.endTimer();
		if ((currFrame%500) == 499)
			printf("Currently generating %1.1f nodes/sec\n", (double)astar.GetNodesExpanded()/totalTime);
		armAngles next = astar.CheckNextNode();
		r->OpenGLDraw(windowID, next);
		r->OpenGLDraw(windowID, goal);
		//astar.GetPath(r, config, goal, ourPath);
	}
}

int MyCLHandler(char *argument[], int maxNumArgs)
{
	if (maxNumArgs <= 1)
		return 0;
	strncpy(gDefaultMap, argument[1], 1024);
	return 2;
}

void MyDisplayHandler(unsigned long windowID, tKeyboardModifier mod, char key)
{
	switch (key)
	{
		case '\t':
			if (mod != kShiftDown)
				SetActivePort(windowID, (GetActivePort(windowID)+1)%GetNumPorts(windowID));
			else
			{
				SetNumPorts(windowID, 1+(GetNumPorts(windowID)%MAXPORTS));
			}
			break;
		case 'p': //unitSims[windowID]->SetPaused(!unitSims[windowID]->GetPaused()); break;
		case 'o':
//			if (unitSims[windowID]->GetPaused())
//			{
//				unitSims[windowID]->SetPaused(false);
//				unitSims[windowID]->StepTime(1.0/30.0);
//				unitSims[windowID]->SetPaused(true);
//			}
			break;
		case ']': absType = (absType+1)%3; break;
		case '[': absType = (absType+4)%3; break;
			//		case '{': unitSim->setPaused(true); unitSim->offsetDisplayTime(-0.5); break;
			//		case '}': unitSim->offsetDisplayTime(0.5); break;
		default:
			break;
	}
}


void MyKeyHandler(unsigned long, tKeyboardModifier, char key)
{
	static int which = 0;
	if ((key >= '0') && (key <= '9'))
	{
		which = key-'0';
		return;
	}
	
	if (key == 'a')
	{
		std::vector<armRotations> actions;

		armRotations rot;
		rot.SetRotation(which, kRotateCW);
		r->GetActions(config, actions);
		for (unsigned int x = 0; x < actions.size(); x++)
			if (rot == actions[x])
			{
				r->ApplyAction(config, rot);
				ourPath.resize(0);
			}
	}
	if (key == 's')
	{
		std::vector<armRotations> actions;
		
		armRotations rot;
		rot.SetRotation(which, kRotateCCW);
		r->GetActions(config, actions);
		for (unsigned int x = 0; x < actions.size(); x++)
			if (rot == actions[x])
			{
				r->ApplyAction(config, rot);
				ourPath.resize(0);
			}
	}
}

bool MyClickHandler(unsigned long , int x, int y, point3d loc, tButtonType whichButton, tMouseEventType mouseEvent)
{
	printf("Hit %d/%d (%f, %f)\n", x, y, loc.x, loc.y);
	if ((mouseEvent != kMouseDown) || (whichButton != kRightButton))
		return false;
	goal.SetGoal(loc.x, loc.y);
	validSearch = astar.InitializeSearch(r, config, goal, ourPath);
	if (validSearch)
	{
		std::cout << "Starting search between: " << config << " and " << goal << std::endl;
		totalTime = 0;
	}
//	static point3d oldloc(0, 0, 0);
//
//	if (oldloc.x != 0)
//	{
//		r->AddObstacle(line2d(recVec(oldloc.x, oldloc.y, 0), recVec(loc.x, loc.y, 0)));
//		oldloc = loc;
//		return true;
//	}
//	oldloc = loc;
//	return false;

	return true;
}

#if 0
void AddToMinHeap( std::vector<armAngles> &heap, armAngles &arm,
		   float *distances )
{
	unsigned i;

	// need to increase size, although we don't actually care
	// about the new content yet
	heap.push_back( arm );
	for( i = heap.size(); i > 1; i >>= 1 ) {
		if( ArmDistance( arm, distances )
		    >= ArmDistance( heap[ ( i >> 1 ) - 1 ], distances ) ) {
			break;
		}
		heap[ i - 1 ] = heap[ ( i >> 1 ) - 1 ];
	}
	heap[ i - 1 ] = arm;
}

armAngles GetFromMinHeap( std::vector<armAngles> &heap, float *distances )
{
	unsigned c; // child index
	armAngles ret;

	ret = heap[ 1 - 1 ];

	// item heap.size() starts as the root (1) but it
	// must be pushed down if it is too large
	c = 1 << 1;
	while( c <= heap.size() - 1 ) {
		if( c + 1 <= heap.size() - 1
		    && ArmDistance( heap[ c - 1 ], distances )
		    > ArmDistance( heap[ c + 1 - 1 ], distances ) ) {
			// child c is bigger than child c+1, so use c+1
			++c;
		}
		if( ArmDistance( heap[ heap.size() - 1 ], distances )
		    <= ArmDistance( heap[ c - 1 ], distances ) ) {
			// new root <= than child, so it stops moving down
			break;
		}

		// root is pushed down past child, so child
		// item is now the parent
		heap[ ( c >> 1 ) - 1 ] = heap[ c - 1 ];

		// root has a new potential location, get new child
		c <<= 1;
	}

	// we found the child of our final location, put item
	// in its place
	heap[ ( c >> 1 ) - 1 ] = heap[ heap.size() - 1 ];
	heap.pop_back();

	return ret;
}
#endif
