/*
 * (C) 2014 Guhl <guhl@dershampoonierte.com>
 * based on tohkbd2-daemon (C) 2014 Kimmo Lindholm <kimmo.lindholm@gmail.com> Kimmoli
 *
 */

#ifndef HWKEYBOARD_H
#define HWKEYBOARD_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#include <QTime>
#include <QTimer>
#include <QThread>
#include "worker.h"
#include <QList>
#include <QPair>

#include <mlite5/MGConfItem>

#define SERVICE_NAME "com.dershampoonierte.hwkbd"
#define EVDEV_OFFSET (8)

/* main class */

class Hwkbd: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)

public:
    Hwkbd();

    ~Hwkbd()
    {
        worker->abort();
        thread->wait();
        delete thread;
        delete worker;
    }

public slots:
    /* dbus signal handler slots */
    void handleDisplayStatus(const QDBusMessage& msg);
    void handleKeyboardStatus(const QDBusMessage& msg);

    void handleDconfCurrentLayout();

    /* DBUS */
    void fakeVkbChange(const QDBusMessage& msg);
    void testSwitchEvent(const QDBusMessage& msg);

signals:

    void keyboardConnectedChanged(bool);

private:

    QString readOneLineFromFile(QString name);
    void changeActiveLayout();

    QThread *thread;
    Worker *worker;

    QMutex mutex;

    bool displayIsOn;

    QString currentActiveLayout;

    bool vkbLayoutIsHwkbd;
    QProcess *process;
};



#endif // HWKEYBOARD_H
