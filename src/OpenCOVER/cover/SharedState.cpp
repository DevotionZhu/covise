/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#include "SharedState.h"
#include <vrbclient/regClass.h>
#include <vrbclient/VrbClientRegistry.h>
#include <coVRCommunication.h>

namespace opencover
{
template <>
void serialize<std::vector<std::string>>(covise::TokenBuffer &tb, const std::vector<std::string> &value)
{
    uint32_t size = value.size();
    tb << size;
    for (size_t i = 0; i < size; i++)
    {
        tb << value[i];
    }
}

template <>
void deserialize<std::vector<std::string>>(covise::TokenBuffer &tb, std::vector<std::string> &value)
{
    uint32_t size;
    tb >> size;
    value.clear();
    value.resize(size);
    for (size_t i = 0; i < size; i++)
    {
        std::string path;
        tb >> path;
        value[i] = path;
    }
}

SharedStateBase::SharedStateBase(std::string name)
    : m_registry(coVRCommunication::instance()->registry)
    , variableName(name)
{
}

SharedStateBase::~SharedStateBase()
{
    m_registry->unsubscribeVar(className, variableName);
}

void SharedStateBase::subscribe(covise::TokenBuffer && val)
{
    m_registry->subscribeVar(className, variableName, std::move(val), this);
}
void SharedStateBase::setVar(covise::TokenBuffer && val)
{
    m_registry->setVar(className, variableName, std::move(val));
}
void SharedStateBase::setUpdateFunction(std::function<void ()> function)
{
    updateCallback = function;
}

bool SharedStateBase::valueChangedByOther() const
{
    return valueChanged;
}

std::string SharedStateBase::getName() const
{
    return variableName;
}

void SharedStateBase::update(clientRegVar *theChangedVar)
{
    if (theChangedVar->getName()!= variableName)
    {
        return;
    }
    theChangedVar->getValue().rewind();
    deserializeValue(theChangedVar->getValue());
    valueChanged = true;
    if (updateCallback != nullptr)
    {
        updateCallback();
    }
}

}
