#ifndef DELAYEDSAVEMANAGERIMPL_H
#define DELAYEDSAVEMANAGERIMPL_H

#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QDateTime>

#include "mainwindow.h"
#include "DelayedSaveManager.h"
#include "Saver.h"

class DelayedSaveManagerImpl;

class DelayedSaveThread : public QThread
{
    Q_OBJECT

public:
    static constexpr unsigned long WAIT_TIME_MILLIS = 1000L;

    typedef qint64 TIMESTAMP_TYPE;
    static constexpr TIMESTAMP_TYPE A_VERY_OBSOLETE_TIMESTAMP=0;

    DelayedSaveThread(DelayedSaveManagerImpl* delayedSaveManagerImpl);
    virtual ~DelayedSaveThread();

    void run() override;

    void deferSaveUntil(TIMESTAMP_TYPE wakeTime);
    void startSavingNow();

    void wakeThreadAndJoinThread();

private:
    DelayedSaveManagerImpl* delayedSaveManagerImpl;
    QMutex* mutex;
    QWaitCondition* waitCondition;
    volatile bool saveNow;
    volatile bool defer;
    volatile TIMESTAMP_TYPE wakeTime;
};

class DelayedSaveManagerImpl : public DelayedSaveManager
{
public:
    DelayedSaveManagerImpl();
    virtual ~DelayedSaveManagerImpl();
    virtual void setSaver(Saver* saver);
    virtual void start();
    virtual void delayedSave(DATA_SERIAL_TYPE dataSerial, bool focusOnTunnel, std::string tunnelNameToFocus);
    virtual bool appExiting();

    typedef DelayedSaveThread::TIMESTAMP_TYPE TIMESTAMP_TYPE;

    static constexpr DATA_SERIAL_TYPE INITIAL_DATA_SERIAL=0;
    bool isExiting();
    Saver* getSaver();
    static TIMESTAMP_TYPE getTime();

    bool needsFocusOnTunnel();
    std::string getTunnelNameToFocus();

private:
    Saver* saver;
    bool isSaverValid();

    volatile DATA_SERIAL_TYPE lastDataSerialSeen;

    static constexpr TIMESTAMP_TYPE A_VERY_OBSOLETE_TIMESTAMP=DelayedSaveThread::A_VERY_OBSOLETE_TIMESTAMP;
    TIMESTAMP_TYPE lastSaveStartedTimestamp;

    volatile bool exiting;
    DelayedSaveThread* thread;
    void wakeThreadAndJoinThread();

    volatile bool focusOnTunnel;
    std::string tunnelNameToFocus;
};

#endif // DELAYEDSAVEMANAGERIMPL_H
