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
    // Database fields
    QString token {"[INVALID]"};
    QString name {"Unknown"};
    QString config;
    QString place;

    // For GUI
    bool cameraEnabled {false};
    bool isConnected {false};
    bool isWorking {false};
    bool isValid {false};
    bool canWork {false};
    std::list<QString> objects;
};

#endif // COMMONSTRUCTS_H
