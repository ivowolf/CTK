/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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


#include "ctkExampleDicomAppLogic_p.h"
#include <QtPlugin>
#include <QRect>

ctkExampleDicomAppLogic::ctkExampleDicomAppLogic(ServiceAccessor<ctkDicomHostInterface>* host)
  : host(host)
{
}

ctkExampleDicomAppLogic::~ctkExampleDicomAppLogic()
{
  delete host;
}

ctkDicomWG23::State ctkExampleDicomAppLogic::getState()
{
  return ctkDicomWG23::IDLE;
}

bool ctkExampleDicomAppLogic::setState(ctkDicomWG23::State newState)
{
  qDebug() << "setState called";
  return false;
}

bool ctkExampleDicomAppLogic::bringToFront(const QRect& requestedScreenArea)
{
  return false;
}

void ctkExampleDicomAppLogic::do_something()
{
  QRect preferred;
  QRect rect = host->call()->getAvailableScreen(preferred);
}