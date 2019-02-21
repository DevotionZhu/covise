/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#ifndef VRB_SERVER_H
#define VRB_SERVER_H

#include <QObject>
#include <QString>
#include <set>

namespace covise
{
class ServerConnection;
class Connection;
class ConnectionList;
class Message;
}
class QSocketNotifier;
class VRBSClient;
class VrbServerRegistry;

#ifdef Q_MOC_RUN
#define GUI
#endif
//
//
//
#ifndef GUI
class VRBServer
#else
class VRBServer : public QObject
#endif
{

#ifdef GUI
    Q_OBJECT

private slots:
#endif
    void processMessages();

public:
    VRBServer();
    ~VRBServer();
    void loop();
    int openServer();
    void closeServer();
    VrbServerRegistry *registry = nullptr;

private:
    covise::ServerConnection *sConn = nullptr;
    QSocketNotifier *serverSN = nullptr;
    covise::ConnectionList *connections = nullptr;
    int port; // port Number (default: 31800) covise.config: VRB.TCPPort
    void handleClient(covise::Message *);
    int createSession();
    void RerouteRequest(const char *location, int type, int senderId, int recvVRBId, QString filter, QString path);
    covise::Message *msg = nullptr;
    bool requestToQuit = false;
    VRBSClient *currentFileClient = nullptr;
    char *currentFile = nullptr;
    std::set<int> sessions;
};
#endif
