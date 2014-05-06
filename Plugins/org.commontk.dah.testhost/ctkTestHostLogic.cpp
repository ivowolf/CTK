// Qt includes
#include <QHash>

// ctk includes
#include "ctkTestHostLogic.h"
#include "ctkExampleDicomHost.h"
#include "ctkDicomAvailableDataHelper.h"

#include "ctkTestQueue.h"

ctkTestHostLogic::ctkTestHostLogic(QString appFileName, QString aDICOMTestDataPath,
                                   ctkHostedAppPlaceholderWidget* placeHolder, 
                                   QWidget* placeHolderForControls, 
                                   int hostPort, int appPort) : 
  AppFileName(appFileName),
  QObject(placeHolder), 
  PlaceHolderForHostedApp(placeHolder),
  PlaceHolderForControls(placeHolderForControls),
  DICOMTestDataPath(aDICOMTestDataPath)
{
  this->TestQueue = new TestQueueManager(this);

  this->Host = new ctkExampleDicomHost(PlaceHolderForHostedApp, hostPort, appPort);

  Data = new ctkDicomAppHosting::AvailableData;

  disconnect(this->Host,SIGNAL(startProgress()),this->Host,SLOT(onStartProgress()));
  connect(this->Host,SIGNAL(appReady()),this,SLOT(onAppReady()), Qt::QueuedConnection);
  connect(this->Host,SIGNAL(startProgress()),this,SLOT(startProgress()), Qt::QueuedConnection);
  connect(this->PlaceHolderForHostedApp,SIGNAL(resized()),this,SLOT(placeHolderResized()));

  connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()) );
}

ctkTestHostLogic::~ctkTestHostLogic()
{
  delete Data;
  delete TestQueue;
}

void ctkTestHostLogic::startTest()
{
  connect(&this->Host->getAppProcess(),SIGNAL(readyReadStandardOutput()),this,SLOT(outputMessageFromHostedApp()));
  connect(&this->Host->getAppProcess(),SIGNAL(error(QProcess::ProcessError)),SLOT(appProcessError(QProcess::ProcessError)));
  connect(&this->Host->getAppProcess(),SIGNAL(stateChanged(QProcess::ProcessState)),SLOT(appProcessStateChanged(QProcess::ProcessState)));

  connect(this->getHost(),SIGNAL(stateChangedReceived(ctkDicomAppHosting::State)),SLOT(stateChangedReceivedViaAbstractHost(ctkDicomAppHosting::State)));
  connect(this->getHost(), SIGNAL(dataAvailable()), SLOT(onDataAvailable()));

  TestQueue->Add("StartApplication", "[starting]",SLOT(startHostedApp()));
  TestQueue->Add("[running]");
  TestQueue->Add("idle");
  TestQueue->Add("[appReady]");

  TestQueue->Add("setState(INPROGRESS)", "inprogress", SLOT(setStateInProgress()), 200);
  TestQueue->Add("[startProgress]");

  TestQueue->Add("Publish data", "[prepareAvailableData] [start]", SLOT(prepareAvailableData()));
  TestQueue->Add("[prepareAvailableData] [end]");
  TestQueue->Add("[publishData] [start]", SLOT(publishData()));
  TestQueue->Add("[publishData] publishData/notifyDataAvailable returned: 1");

  TestQueue->Add("Let app process data", "[dataAvailable]", "");

  TestQueue->Add("Bring to front", "[bringToFront(rect)] [start]", SLOT(bringToFront()));
  TestQueue->Add("[bringToFront(rect)] [end]");

  TestQueue->Add("setState(CANCELED)", "canceled", SLOT(setStateCanceled()), 4000);
  TestQueue->Add("idle");
  TestQueue->Add("[appReady]");

  TestQueue->Add("setState(INPROGRESS)", "inprogress", SLOT(setStateInProgress()), 200);
  TestQueue->Add("[startProgress]");

  TestQueue->Add("Bring to front", "[bringToFront(rect)] [start]", SLOT(bringToFront()));
  TestQueue->Add("[bringToFront(rect)] [end]");

  TestQueue->Add("setState(SUSPENDED)", "suspended", SLOT(setStateSuspended()), 200);
  TestQueue->Add("setState(INPROGRESS)", "inprogress", SLOT(setStateInProgress()), 200);

  TestQueue->Add("setState(CANCELED)", "canceled", SLOT(setStateCanceled()), 2000);
  TestQueue->Add("idle");
  TestQueue->Add("[appReady]");

  TestQueue->Add("setState(EXIT)", "exit", SLOT(setStateExit()), 2000);
  TestQueue->Add("[not running] last exit code 0");

  TestQueue->Add("StartApplication", "[starting]",SLOT(startHostedApp()));
  TestQueue->Add("[running]");
  TestQueue->Add("idle");
  TestQueue->Add("[appReady]");

  TestQueue->Add("setState(EXIT)", "exit", SLOT(setStateExit()), 2000);
  TestQueue->Add("[not running] last exit code 0");

  TestQueue->Add("Quit", "[quitting]", SLOT(quit()), 5000, qApp);

  TestQueue->Apply();
  //QTimer::singleShot(30000, qApp, SLOT(quit()));
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::startHostedApp()
{

  this->Host->StartApplication(this->AppFileName,QString("--testid 1").split(" "));
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::setStateInProgress()
{
  bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::setStateSuspended()
{
  bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::SUSPENDED);
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::setStateCanceled()
{
  bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::CANCELED);
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::setStateExit()
{
  bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::EXIT);
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::stateChangedReceivedViaAbstractHost(ctkDicomAppHosting::State newState)
{
  QHash<ctkDicomAppHosting::State, QString> hash;
  hash[ctkDicomAppHosting::IDLE]="idle";
  hash[ctkDicomAppHosting::INPROGRESS]="inprogress";
  hash[ctkDicomAppHosting::COMPLETED]="completed";
  hash[ctkDicomAppHosting::SUSPENDED]="suspended";
  hash[ctkDicomAppHosting::CANCELED]="canceled";
  hash[ctkDicomAppHosting::EXIT]="exit";

  TestQueue->CheckAndContinue(hash.value(newState,"[unkown]"));
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::onAppReady()
{
  TestQueue->CheckAndContinue("[appReady]");
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::startProgress()
{
  TestQueue->CheckAndContinue("[startProgress]");
}


//----------------------------------------------------------------------------
void ctkTestHostLogic::prepareAvailableData()
{
  TestQueue->CheckAndContinue("[prepareAvailableData] [start]");
  if(Data == NULL)
  {
    LOG << "ctkDicomAppHosting::AvailableData* Data == NULL - cannot prepare AvailableData";
    return;
  }
  if(DICOMTestDataPath.isEmpty())
  {
    LOG << "DICOMTestDataPath not set - cannot prepare AvailableData";
    return;
  }

  QDir dir(DICOMTestDataPath);
  QStringList files = dir.entryList(QDir::Files);
  foreach (const QString &filename, files) {
    ctkDicomAvailableDataHelper::addToAvailableData(*Data, 
      Host->objectLocatorCache(), 
      dir.absoluteFilePath(filename));
    break; // one is enough for the moment
  }
  TestQueue->CheckAndContinue("[prepareAvailableData] [end]");
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::publishData()
{
  TestQueue->CheckAndContinue("[publishData] [start]");
  bool success = Host->publishData(*Data, true);
  TestQueue->CheckAndContinue("[publishData] publishData/notifyDataAvailable returned: ",QString::number(success));
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::onDataAvailable()
{
  TestQueue->CheckAndContinue("[dataAvailable]");
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::bringToFront()
{
  QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
  TestQueue->CheckAndContinue("[bringToFront(rect)] [start]");
  bool broughtToFront = this->Host->getDicomAppService()->bringToFront(rect);
  LOG << "[bringToFront(rect)] returned: " << broughtToFront;
  TestQueue->CheckAndContinue("[bringToFront(rect)] [end]");
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::aboutToQuit()
{
  TestQueue->CheckAndContinue("[quitting]");

  this->Host->exitApplicationBlocking();

  delete this->Host;
  this->Host = 0;
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
void ctkTestHostLogic::outputMessageFromHostedApp()
{
  LOG << "[app] [Output start]";
  LOG << this->Host->processReadAll();
  LOG << "[app] [Output end]";
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::appProcessStateChanged(QProcess::ProcessState state)
{
  QString info;
  switch (state)
    {
    case QProcess::Running:
      info = "[running]";
      break;
    case QProcess::NotRunning:
      if (this->Host->getAppProcess().exitStatus() == QProcess::CrashExit )
      {
        info = "[not running] crashed";
      }
      else
      {
        info = "[not running] last exit code ";
        info.append(QString::number(this->Host->getAppProcess().exitCode()));
      }
      break;
    case QProcess::Starting:
      info = "[starting]";
      break;
    default:
      info = "[unknown]";
      break;
    }
  LOG << "[app] [ProcessState changed] " << info;
  TestQueue->CheckAndContinue(info);
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::appProcessError(QProcess::ProcessError error)
{
  QString info;
  switch (error)
    {
    case QProcess::FailedToStart:
      info = "[failed to start]";
      break;
    case QProcess::Crashed:
      info = "[crash detected]";
      break;
    case QProcess::Timedout:
      info = "[timed out]";
      break;
    case QProcess::UnknownError:
      info = "[unkown error]";
      break;
    default:
      ;
    }
  TestQueue->CheckAndContinue(info);
}
