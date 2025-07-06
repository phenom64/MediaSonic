/*
 * TM & (C) 2025 Syndromatic Ltd. All rights reserved.
 * Designed by Kavish Krishnakumar in Manchester.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "application.h"
#include <QDir>

Application::Application(int &argc, char *argv[])
    : QApplication(argc, argv)
{
    // Unique key for the application
    const QString serverName = "SynOSMusicPlayerServer";

    // Try to connect to the local server
    QLocalSocket socket;
    socket.connectToServer(serverName);

    // If the connection is successful, another instance is running
    if (socket.waitForConnected(500)) {
        _isRunning = true;
    } else {
        // No other instance is running, so start the server
        _isRunning = false;
        localServer = new QLocalServer(this);
        connect(localServer, &QLocalServer::newConnection, this, [=]() {
            // Handle new connections if needed, e.g., to show the existing window
        });
        // Ensure the server is properly cleaned up if it exists
        QLocalServer::removeServer(serverName);
        localServer->listen(serverName);
    }
}

bool Application::isRunning()
{
    return _isRunning;
}
