// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QApplication>
#include <QHash>

//#include <QModelIndex>
//#include <QTreeView>
//#include <QItemSelectionModel>

// ctk includes
#include "ctkTestHostLogic.h"
//#include "ctkHostedAppPlaceholderWidget.h"
//#include "ctkExampleHostControlWidget.h"
#include "ctkExampleDicomHost.h"
#include "ctkDicomAvailableDataHelper.h"

#define LOG qDebug() << QString("[@").append(QTime::currentTime().toString()).append("]")

class TestQueueData
{
public:
  QString Title;
  QObject * Receiver;
  QString Slotname;
  QString ExpectedResult;
  int TimerMsec;

  TestQueueData(QString title, QString expectedResult, QObject * receiver, QString slotname, int timerMsec=0) :
    Title(title), ExpectedResult(expectedResult), Receiver(receiver), Slotname(slotname), TimerMsec(timerMsec)
  {
  }
  void Apply()
  {
    LOG << "[Test: " << Title << "]"; 
    if(Slotname.isEmpty()==false)
      QTimer::singleShot(0, Receiver, Slotname.toAscii());
  }
  bool Check(QString result)
  {
    if(ExpectedResult == result)
    {
      LOG << "[Test: " << Title << "] " << "ok: " << result;
      return true;
    }
    else
    {
      LOG << "[Test: " << Title << "] " << "failed: expected " << ExpectedResult << ", but got " << result; 
      return false;
    }
  }
};

class TestQueueManager
{
  QQueue<TestQueueData> TestQueue;
  QObject* Receiver;
  QString CurrentTestTitel;
public:
  TestQueueManager(QObject* receiver) : Receiver(receiver), CurrentTestTitel("TestQueueManager Main")
  {
  }
  void Add(QString title, QString expectedResult, QString slotname, int timerMsec=0, QObject* receiver=NULL)
  {
    TestQueue.enqueue(TestQueueData(title, expectedResult, (receiver==NULL?Receiver:receiver), slotname, timerMsec));
  }
  void Add(QString expectedResult, QString slotname="", int timerMsec=0, QObject* receiver=NULL)
  {
      TestQueue.enqueue(TestQueueData("", expectedResult, (receiver==NULL?Receiver:receiver), slotname, timerMsec));
  }
  void Apply()
  {
    if(TestQueue.isEmpty()==false)
    {
      if(TestQueue.head().Title.isEmpty())
      {
        TestQueue.head().Title = QString(CurrentTestTitel).append(" (continued)");
      }
      else
        CurrentTestTitel = TestQueue.head().Title;
      TestQueue.head().Apply();
    }
  }
  bool CheckAndContinue(QString result)
  {
    if(TestQueue.isEmpty())
      return false;
    bool res = TestQueue.head().Check(result); 
    TestQueue.dequeue();
    Apply();
    return res;
  }
};

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
  this->TestQueue = new TestQueueManager(this);

  this->Host = new ctkExampleDicomHost(PlaceHolderForHostedApp, hostPort, appPort);
  //this->HostControls = new ctkExampleHostControlWidget(Host, PlaceHolderForControls);

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

  TestQueue->Add("StartApplication", "[starting]",SLOT(startHostedApp()));
  TestQueue->Add("[running]");
  TestQueue->Add("idle");
  TestQueue->Add("[appReady]");

  TestQueue->Add("setState(INPROGRESS)", "inprogress", SLOT(setStateInProgress()), 200);
  TestQueue->Add("[startProgress]");
  TestQueue->Add("Publish data", "[publishSelectedData]", SLOT(publishSelectedData()));

  TestQueue->Add("setState(CANCELED)", "canceled", SLOT(setStateCanceled()), 2000);
  TestQueue->Add("idle");
  TestQueue->Add("[appReady]");

  TestQueue->Add("setState(INPROGRESS)", "inprogress", SLOT(setStateInProgress()), 200);
  TestQueue->Add("[startProgress]");

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
  this->Host->StartApplication(this->AppFileName);
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
//  emit SelectionValid(ValidSelection);
  //if(SendData)
  //{
  //  bool reply = this->Host->getDicomAppService()->setState(ctkDicomAppHosting::INPROGRESS);
  //  LOG << "  setState(INPROGRESS) returned: " << reply;

  //  QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
  //  this->Host->getDicomAppService()->bringToFront(rect);
  //}
}
//----------------------------------------------------------------------------
void ctkTestHostLogic::startProgress()
{
  TestQueue->CheckAndContinue("[startProgress]");
}

//----------------------------------------------------------------------------
void ctkTestHostLogic::publishSelectedData()
{
  TestQueue->CheckAndContinue("[publishSelectedData]");
  if(SendData)
  {
    LOG << "send dataDescriptors";
    bool success = Host->publishData(*Data, LastData);
    if(!success)
    {
      qCritical() << "Failed to publish data";
    }
    LOG << "  notifyDataAvailable returned: " << success;
    SendData=false;

    QRect rect (this->PlaceHolderForHostedApp->getAbsolutePosition());
    this->Host->getDicomAppService()->bringToFront(rect);
  }
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
//ctkExampleHostControlWidget* ctkTestHostLogic::getHostControls()
//{
//  return this->HostControls;
//}

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
