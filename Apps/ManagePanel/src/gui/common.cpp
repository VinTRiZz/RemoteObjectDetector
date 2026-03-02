#include "common.hpp"

namespace Common
{

QUrl createUrl(const QString &serverAddress, const QString &target, const QStringList &args)
{
    return QUrl("http://" + serverAddress + target + args.join("&"));
}

QString getTarget(const QUrl &url)
{
    auto urlString = url.toString(QUrl::RemoveScheme | QUrl::RemovePort | QUrl::RemoveAuthority);
    return urlString;
}



}
