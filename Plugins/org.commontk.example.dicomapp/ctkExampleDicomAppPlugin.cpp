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


#include "ctkExampleDicomAppPlugin_p.h"
#include "ctkExampleDicomAppLogic_p.h"
#include <QtPlugin>
#include <QStringList.h>
#include <QString.h>

ctkExampleDicomAppPlugin* ctkExampleDicomAppPlugin::instance = 0;

ctkExampleDicomAppPlugin::ctkExampleDicomAppPlugin()
  : context(0)
{
}

ctkExampleDicomAppPlugin::~ctkExampleDicomAppPlugin()
{
  
}

void ctkExampleDicomAppPlugin::start(ctkPluginContext* context)
{
  instance = this;
  this->context = context;
  context->registerService(QStringList("ctkDicomAppInterface"), 
    new ctkExampleDicomAppLogic(new ServiceAccessor<ctkDicomHostInterface>(context,"ctkDicomHostInterface")));

  //ctkServiceReference* serviceRef = context->getServiceReference("ctkDicomHostInterface");
  //if (!serviceRef)
  //{
  //  // this will change after merging changes from branch plugin_framework
  //  throw std::runtime_error("No Dicom Host Service found");
  //}
  //ctkDicomHostInterface*
  //  serviceBinding = qobject_cast<ctkDicomHostInterface*>(context->getService(serviceRef));
}

void ctkExampleDicomAppPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

ctkExampleDicomAppPlugin* ctkExampleDicomAppPlugin::getInstance()
{
  return instance;
}

ctkPluginContext* ctkExampleDicomAppPlugin::getPluginContext() const
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_example_dicomapp, ctkExampleDicomAppPlugin)


