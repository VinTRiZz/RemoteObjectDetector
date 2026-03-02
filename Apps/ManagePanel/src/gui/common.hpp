#pragma once

#include <QString>
#include <QUrl>

namespace Common
{

QUrl createUrl(const QString& serverAddress, const QString& target, const QStringList& args = {});

QString getTarget(const QUrl &url);

}
