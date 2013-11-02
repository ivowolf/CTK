// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QApplication>
#include <QModelIndex>
#include <QTreeView>
#include <QItemSelectionModel>

// ctk includes
#include "ctkTestHostLogic.h"
//#include "ctkHostedAppPlaceholderWidget.h"
//#include "ctkExampleHostControlWidget.h"
#include "ctkExampleDicomHost.h"
#include "ctkDicomAvailableDataHelper.h"

ctkTestHostLogic::ctkTestHostLogic(QString appFileName, 
                                   ctkHostedAppPlaceholderWidget* placeHolder, 
                                   QWidget* placeHolderForControls, 
                                   int hostPort, int appPort) : 
  AppFileName(appFileName),
  QObject(placeHolder), 
  PlaceHolderForHostedApp(placeHolder),
  PlaceHolderForControls(placeHolderForControls),
//  ValidSelection(false),
  LastData(false),
  SendData(false)
{
  this->Host = new ctkExampleDicomHost(PlaceHolderForHostedApp, hostPort, appPort);
  //this->HostControls = new ctkExampleHostControlWidget(Host, PlaceHolderForControls);

  Data = new ctkDicomAppHosting::AvailableData;

  disconnect(this->Host,SIGNAL(startProgress()),this->Host,SLOT(onStartProgress()));
  connect(this->Host,SIGNAL(appReady()),this,SLOT(onAppReady()), Qt::QueuedConnection);
  connect(this->Host,SIGNAL(startProgress()),this,SLOT(publishSelectedData()), Qt::QueuedConnection);
  connect(this->PlaceHolderForHostedApp,SIGNAL(resized()),this,SLOT(placeHolderResized()));

  connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()) );
}

ctkTestHostLogic::~ctkTestHostLogic()
{
  delete Data;
}

void ctkTestHostLogic::aboutToQuit()
{
  this->Host->exitApplicationBlocking();

  delete this->Host;
  this->Host = 0;
}

void ctkTestHostLogic::startTest()
{
  this->Host->StartApplication(this->AppFileName);
  connect(&this->Host->getAppProcess(),SIGNAL(readyReadStandardOutput()),this,SLOT(outputMessage()));
  connect(&this->Host->getAppProcess(),SIGNAL(error(QProcess::ProcessError)),SLOT(appProcessError(QProcess::ProcessError)));
  connect(&this->Host->getAppProcess(),SIGNAL(stateChanged(QProcess::ProcessState)),SLOT(appProcessStateChanged(QProcess::ProcessState)));
}

void ctkTestHostLogic::sendData(ctkDicomAppHosting::AvailableData& data, bool lastData)
{
 if ((this->Host))// && (this->HostControls->validAppFileName()) /*&& (ValidSelection)*/)
  {
    *Data = data;
    LastData = lastData;
 
    SendData = true;
    if(this->Host->getApplicationState() == ctkDicomAppHosting::EXIT)
    {
      this->Host->StartApplication(this->AppFileName);
      //forward output to textedit
      connect(&this->Host->getAppProcess(),SIGNAL(readyReadStandardOutput()),this,SLOT(outputMessage()));
    }
    if(this->Host->getApplicationState() == ctkDicomAppHosting::IDLE)
    {
      bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
    }
    if(this->Host->getApplicationState() == ctkDicomAppHosting::INPROGRESS)
    {
      publishSelectedData();
    }
  }
}

void ctkTestHostLogic::onAppReady()
{
  this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
//  emit SelectionValid(ValidSelection);
  //if(SendData)
  //{
  //  bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
  //  qDebug() << "  setState(INPROGRESS) returned: " << reply;

  //  QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
  //  this->Host->getDicomAppService()->bringToFront(rect);
  //}
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::publishSelectedData()
{
  if(SendData)
  {
    qDebug()<<"send dataDescriptors";
    bool success = Host->publishData(*Data, LastData);
    if(!success)
    {
      qCritical() << "Failed to publish data";
    }
    qDebug() << "  notifyDataAvailable returned: " << success;
    SendData=false;

    QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
    this->Host->getDicomAppService()->bringToFront(rect);
  }
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::placeHolderResized()
{
  ///the following does not work (yet)
  if((this->Host) && (this->Host->getApplicationState() != ctkDicomAppHosting::EXIT))
  {
    QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
    this->Host->getDicomAppService()->bringToFront(rect);
  }
}

//----------------------------------------------------------------------------
ctkExampleDicomHost* ctkTestHostLogic::getHost()
{
  return this->Host;
}

//----------------------------------------------------------------------------
//ctkExampleHostControlWidget* ctkTestHostLogic::getHostControls()
//{
//  return this->HostControls;
//}
