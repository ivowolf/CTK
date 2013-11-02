/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

// Qt includes
#include <QtPlugin>

// CTK includes
#include "ctkTestDicomHostPlugin_p.h"

ctkTestDicomHostPlugin* ctkTestDicomHostPlugin::Instance = 0;

//----------------------------------------------------------------------------
ctkTestDicomHostPlugin::ctkTestDicomHostPlugin()
  : Context(0)
{
}

//----------------------------------------------------------------------------
ctkTestDicomHostPlugin::~ctkTestDicomHostPlugin()
{
  
}

//----------------------------------------------------------------------------
void ctkTestDicomHostPlugin::start(ctkPluginContext* context)
{
  ctkTestDicomHostPlugin::Instance = this;
  this->Context = context;
}

//----------------------------------------------------------------------------
void ctkTestDicomHostPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

//----------------------------------------------------------------------------
ctkTestDicomHostPlugin* ctkTestDicomHostPlugin::getInstance()
{
  return ctkTestDicomHostPlugin::Instance;
}

//----------------------------------------------------------------------------
ctkPluginContext* ctkTestDicomHostPlugin::getPluginContext() const
{
  return this->Context;
}

Q_EXPORT_PLUGIN2(org_commontk_dah_testhost, ctkTestDicomHostPlugin)


