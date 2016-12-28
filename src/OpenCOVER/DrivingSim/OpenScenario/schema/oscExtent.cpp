/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl - 2.1.txt.

* License: LGPL 2 + */

#include "schema/oscExtent.h"

using namespace OpenScenario;
Enum_domain_time_distanceType::Enum_domain_time_distanceType()
{
addEnum("time", oscExtent::time);
addEnum("distance", oscExtent::distance);
}
Enum_domain_time_distanceType *Enum_domain_time_distanceType::instance()
{
	if (inst == NULL)
	{
		inst = new Enum_domain_time_distanceType();
	}
	return inst;
}
Enum_domain_time_distanceType *Enum_domain_time_distanceType::inst = NULL;