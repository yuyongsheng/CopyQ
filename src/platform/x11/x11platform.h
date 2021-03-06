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

#ifndef X11PLATFORM_H
#define X11PLATFORM_H

#include "platform/platformnativeinterface.h"

#include <QKeyEvent>
#include <QString>

#include <memory>

class X11DisplayGuard;

class X11Platform : public PlatformNativeInterface
{
public:
    X11Platform();

    ~X11Platform();

    PlatformWindowPtr getWindow(WId winId) override;

    PlatformWindowPtr getCurrentWindow() override;

    bool canGetWindowTitle() override { return true; }

    bool canAutostart() override;

    /**
     * Return true only if "copyq.desktop" file exists in "autostart" directory of current user and
     * it doesn't contain "Hidden" property or its value is false.
     */
    bool isAutostartEnabled() override;

    /**
     * Replace "Hidden" property in current user's autostart "copyq.desktop" file
     * (create the file if it doesn't exist).
     *
     * Additionally, replace "Exec" property with current application path.
     */
    void setAutostartEnabled(bool) override;

    QCoreApplication *createConsoleApplication(int &argc, char **argv) override;

    QApplication *createServerApplication(int &argc, char **argv) override;

    QApplication *createMonitorApplication(int &argc, char **argv) override;

    QCoreApplication *createClientApplication(int &argc, char **argv) override;

    void loadSettings() override {}

    PlatformClipboardPtr clipboard() override;

    int keyCode(const QKeyEvent &event) override { return event.key(); }

    QStringList getCommandLineArguments(int argc, char **argv) override;

    bool findPluginDir(QDir *pluginsDir) override;

    QString defaultEditorCommand() override;

    QString translationPrefix() override;

    QString themePrefix() override { return QString(); }

private:
    std::shared_ptr<X11DisplayGuard> d;
};

#endif // X11PLATFORM_H
