#ifndef CTKTESTHOSTLOGIC_H
#define CTKTESTHOSTLOGIC_H

#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QQueue>

#include "ctkDicomAppHostingTypes.h"

#include <org_commontk_dah_testhost_Export.h>

class ctkHostedAppPlaceholderWidget;

class QModelIndex;
class QItemSelection;

class ctkExampleDicomHost;

class TestQueueManager;

class org_commontk_dah_testhost_EXPORT ctkTestHostLogic :
  public QObject
{
  Q_OBJECT
public:
  ctkTestHostLogic(QString appFileName, QString aDICOMTestDataPath="",
    ctkHostedAppPlaceholderWidget* = NULL, QWidget* placeHolderForControls = NULL, int hostPort = 8080, int appPort = 8081);
  virtual ~ctkTestHostLogic();
  ctkExampleDicomHost* getHost();
public slots:
  void startTest();

  void appProcessError(QProcess::ProcessError error);
  void appProcessStateChanged(QProcess::ProcessState state);

  void outputMessageFromHostedApp();

protected slots:
  void prepareAvailableData();
  void publishData();
  void bringToFront();
  void onAppReady();
  void startProgress();
  void placeHolderResized();
  void aboutToQuit();
  void stateChangedReceivedViaAbstractHost(ctkDicomAppHosting::State);
  void onDataAvailable();

  void startHostedApp();
  void setStateInProgress();
  void setStateSuspended();
  void setStateCanceled();
  void setStateExit();
protected:
  ctkExampleDicomHost* Host;
  ctkHostedAppPlaceholderWidget* PlaceHolderForHostedApp;
  QWidget* PlaceHolderForControls;
  QString AppFileName;
  ctkDicomAppHosting::AvailableData* Data;
  QString DICOMTestDataPath;

  TestQueueManager* TestQueue;
};

#endif
