/*
 * (C) 2014 Guhl <guhl@dershampoonierte.com>
 * based on tohkbd2-daemon (C) 2014 Kimmo Lindholm <kimmo.lindholm@gmail.com> Kimmoli
 *
 */

#include <QtCore/QCoreApplication>
#include <QDBusMessage>
#include <QThread>

#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <QTimer>

#include "hwkeyboard.h"

#include <mlite5/MGConfItem>

/* Main
 */
Hwkbd::Hwkbd()
{
    vkbLayoutIsHwkbd = false;
    currentActiveLayout = QString();
    displayIsOn = false;

    thread = new QThread();
    worker = new Worker();

    worker->moveToThread(thread);
    connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));
    connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);
}

/* Handle state change of phone display
 * Turn keyboard backlight on and off
 */
void Hwkbd::handleDisplayStatus(const QDBusMessage& msg)
{
    QList<QVariant> args = msg.arguments();
    const char *turn = qPrintable(args.at(0).toString());

    printf("Display status changed to \"%s\"\n", turn);
    if (strcmp(turn, "on") == 0)
    {
        displayIsOn = true;
    }
    else if (strcmp(turn, "off") == 0)
    {
        displayIsOn = false;
    }
}

/* Handle state change of hardware keyboard
 * Change keyboard layout
 */
void Hwkbd::handleKeyboardStatus(const QDBusMessage& msg)
{
    QList<QVariant> args = msg.arguments();
    const char *status = qPrintable(args.at(0).toString());

    printf("Keyboard status changed to \"%s\"\n", status);
    if (strcmp(status, "closed") == 0)
    {
        vkbLayoutIsHwkbd = false;
    }
    else if (strcmp(status, "open") == 0)
    {
        vkbLayoutIsHwkbd = true;
    }
    changeActiveLayout();
}


/* Read first line from a text file
 * returns empty QString if failed
 */
QString Hwkbd::readOneLineFromFile(QString name)
{
    QString line;

    QFile inputFile( name );

    if ( inputFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
       QTextStream in( &inputFile );
       line = in.readLine();
       inputFile.close();
    }

    return line;
}

/* Change virtual keyboard active layout,
 * uses private: vkbLayoutIsHwkbd
 * true = change to hwkbd.qml
 * false = change to last non-hwkbd layout
 *
 * TODO: A separate daemon for this
 */
void Hwkbd::changeActiveLayout()
{
    printf("Getting current vkb layout\n");

    process = new QProcess();
    QObject::connect(process, SIGNAL(readyRead()), this, SLOT(handleDconfCurrentLayout()));

    process->start("/usr/bin/dconf read /sailfish/text_input/active_layout");
}

void Hwkbd::handleDconfCurrentLayout()
{
    QByteArray ba = process->readAll();

    ba.replace('\n', QString());
    QString __currentActiveLayout = QString(ba);

    printf("Current layout is %s\n", qPrintable(__currentActiveLayout));

    if (__currentActiveLayout.contains("hwkbd.qml") && vkbLayoutIsHwkbd)
    {
        return;
    }
    else if (!__currentActiveLayout.contains("hwkbd.qml"))
    {
        if (__currentActiveLayout.contains("qml"))
            currentActiveLayout = __currentActiveLayout;
    }

    QThread::msleep(100);
    process->terminate();

    process = new QProcess();

    if (vkbLayoutIsHwkbd)
    {
        printf("Changing to hwkbd\n");
        process->start("/usr/bin/dconf write /sailfish/text_input/active_layout \"'hwkbd.qml'\"");
    }
    else if (currentActiveLayout.contains("qml"))
    {
        printf("Changing to %s\n", qPrintable(currentActiveLayout));
        process->start(QString("/usr/bin/dconf write /sailfish/text_input/active_layout \"%1\"").arg(currentActiveLayout));
    }

    QThread::msleep(100);
    process->terminate();
}

/** DBUS Test methods */

/* dbus-send --system --print-reply --dest=com.dershampoonierte.hwkbd / com.dershampoonierte.hwkbd.fakeVkbChange boolean:true
 */
void Hwkbd::fakeVkbChange(const QDBusMessage& msg)
{
    QList<QVariant> args = msg.arguments();

    printf("got fake vkb-change\n");

    vkbLayoutIsHwkbd = args.at(0).toBool();
    changeActiveLayout();

}

void Hwkbd::testSwitchEvent(const QDBusMessage &msg)
{
    QList<QVariant> args = msg.arguments();

    if (args.count() == 2)
    {
        printf("setting %d to %d\n", args.at(0).toInt(), args.at(1).toInt());
//        uinputif->sendUinputSwitch(args.at(0).toInt(), args.at(1).toInt());
//        uinputif->synUinputDevice();
    }
}
