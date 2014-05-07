#ifndef CTKTESTTESTQUEUE_H
#define CTKTESTTESTQUEUE_H

// Qt includes
#include <QDebug>

#include <stdexcept>

#define LOG qDebug() << QString("[@").append(QTime::currentTime().toString()).append("]").toStdString().c_str()

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
      QTimer::singleShot(TimerMsec, Receiver, Slotname.toAscii());
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
  bool AbortWhenFailed;
  bool HasFailed;
public:
  TestQueueManager(QObject* receiver, bool abortWhenFailed=true)
    : Receiver(receiver), CurrentTestTitel("TestQueueManager Main"), AbortWhenFailed(abortWhenFailed), HasFailed(false)
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
  bool CheckAndContinue(QString result, QString optionalResultParam="")
  {
    if(TestQueue.isEmpty())
      return false;
    bool res = TestQueue.head().Check(result.append(optionalResultParam));
    HasFailed |= !res;
    if(HasFailed && AbortWhenFailed)
    {
      throw std::logic_error( "Test failed." );
    }
    TestQueue.dequeue();
    Apply();
    return res;
  }
  bool Failed() const
  {
    return HasFailed;
  }
};
#endif
