#ifndef COMMONSTRUCTS_H
#define COMMONSTRUCTS_H

#include <QString>

#include <map>
#include <list>

struct ImageObject
{
    QString name;
    QString imagePath;
};

struct ConnectedDevice
{
    QString token;
    QString name;
    bool isConnected;
    bool cameraEnabled;
    std::map<std::string, std::string> statusMap;
    std::list<QString> objects;
};

#endif // COMMONSTRUCTS_H
