#include "myFactory.h"
#include "Maneuver.h"
#include "Act.h"
#include "Trajectory.h"
#include "FollowTrajectory.h"
#include <DrivingSim/OpenScenario/oscFactories.h>

myFactory::myFactory(){}
myFactory::~myFactory(){}

OpenScenario::oscObjectBase *myFactory::create(const std::string &name)
{
	if (name == "oscManeuver")
	{
		return new Maneuver();
	}
	if (name == "oscAct")
	{
		return new Act();
	}
		if (name == "oscTrajectory")
	{
		return new Trajectory();
	}
	if (name == "oscFollowTrajectory")
	{
		return new FollowTrajectory();
	}
	return OpenScenario::oscFactories::staticObjectFactory.create(name);
}
