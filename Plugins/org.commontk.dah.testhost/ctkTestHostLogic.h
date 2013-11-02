#ifndef CTKDICOMHOSTMAINLOGIC_H
#define CTKDICOMHOSTMAINLOGIC_H

#include <QObject>
#include <QStringList>

#include "ctkDicomAppHostingTypes.h"

#include <org_commontk_dah_testhost_Export.h>

class ctkHostedAppPlaceholderWidget;

class QModelIndex;
class QItemSelection;

class ctkExampleDicomHost;

class org_commontk_dah_testhost_EXPORT ctkTestHostLogic :
  public QObject
{
  Q_OBJECT
public:
  ctkTestHostLogic(QString appFileName, 
    ctkHostedAppPlaceholderWidget* = NULL, QWidget* placeHolderForControls = NULL, int hostPort = 8080, int appPort = 8081);
  virtual ~ctkTestHostLogic();
  ctkExampleDicomHost* getHost();
//  ctkTestHostControlWidget* getHostControls();
public slots:
  void startTest();
  void sendData(ctkDicomAppHosting::AvailableData& data, bool lastData);
protected slots:
  void publishSelectedData();
  void onAppReady();
  void placeHolderResized();
  void aboutToQuit();
protected:
  ctkExampleDicomHost* Host;
//  ctkTestHostControlWidget* HostControls;
  ctkHostedAppPlaceholderWidget* PlaceHolderForHostedApp;
  QWidget* PlaceHolderForControls;
  QString AppFileName;
  ctkDicomAppHosting::AvailableData* Data;
  //bool ValidSelection;
  bool LastData;
  bool SendData;
};

#endif
