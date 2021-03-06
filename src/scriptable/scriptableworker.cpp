/*
    Copyright (c) 2017, Lukas Holecek <hluk@email.cz>

    This file is part of CopyQ.

    CopyQ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CopyQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CopyQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "scriptableworker.h"

#include "common/action.h"
#include "common/client_server.h"
#include "common/clientsocket.h"
#include "common/commandstatus.h"
#include "common/log.h"
#include "../qt/bytearrayclass.h"

#include <QApplication>
#include <QObject>
#include <QScriptEngine>

Q_DECLARE_METATYPE(QByteArray*)

ScriptableWorkerSocketGuard::ScriptableWorkerSocketGuard(const ClientSocketPtr &socket)
    : m_socket(socket)
{
}

ScriptableWorkerSocketGuard::~ScriptableWorkerSocketGuard()
{
    m_socket = nullptr;
}

ClientSocket *ScriptableWorkerSocketGuard::socket() const
{
    return m_socket.get();
}

ScriptableWorker::ScriptableWorker(
        MainWindow *mainWindow,
        const ClientSocketPtr &socket,
        const QString &pluginScript)
    : QRunnable()
    , m_wnd(mainWindow)
    , m_socketGuard(new ScriptableWorkerSocketGuard(socket))
    , m_pluginScript(pluginScript)
{
}

ScriptableWorker::~ScriptableWorker()
{
}

void ScriptableWorker::run()
{
    auto socket = m_socketGuard->socket();

    setCurrentThreadName("Script-" + QString::number(socket->id()));

    QScriptEngine engine;
    ScriptableProxy proxy(m_wnd);
    Scriptable scriptable(&proxy, m_pluginScript);
    scriptable.initEngine(&engine);

    QObject::connect( &proxy, SIGNAL(sendMessage(QByteArray,int)),
                      socket, SLOT(sendMessage(QByteArray,int)) );

    QObject::connect( &scriptable, SIGNAL(sendMessage(QByteArray,int)),
                      socket, SLOT(sendMessage(QByteArray,int)) );
    QObject::connect( socket, SIGNAL(messageReceived(QByteArray,int)),
                      &scriptable, SLOT(onMessageReceived(QByteArray,int)) );

    QObject::connect( socket, SIGNAL(disconnected()),
                      &scriptable, SLOT(onDisconnected()) );
    QObject::connect( socket, SIGNAL(connectionFailed()),
                      &scriptable, SLOT(onDisconnected()) );

    QMetaObject::invokeMethod(socket, "start", Qt::QueuedConnection);

    while ( scriptable.isConnected() )
        QCoreApplication::processEvents();

    QMetaObject::invokeMethod(m_socketGuard, "deleteLater", Qt::QueuedConnection);
}
