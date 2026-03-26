#include "serverlistmodel.hpp"

ServerListModel::ServerListModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant ServerListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        switch (section)
        {
        case 0: return "Имя сервера";
        case 1: return "IP адрес";
        }
        return {};
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

int ServerListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_servers.size();
}

int ServerListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant ServerListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_servers.size() || index.row() < 0)
        return QVariant();

    if (role == Roles::ServerAddress) {
        auto serverIt = m_servers.begin();
        std::advance(serverIt, index.row());
        return serverIt->second;
    }

    if (role == Qt::DisplayRole) {
        auto serverIt = m_servers.begin();
        std::advance(serverIt, index.row());
        switch (index.column())
        {
        case 0: return serverIt->first;
        case 1: return serverIt->second;
        }
        return {};
    }

    return QVariant();
}

void ServerListModel::addServer(const QString &serverName, const QString &serverAddress)
{
    auto replaceIt = m_servers.find(serverName);
    if (replaceIt != m_servers.end()) {
        replaceIt->second = serverAddress;
        auto indxRow = std::distance(m_servers.begin(), replaceIt);
        emit dataChanged(index(indxRow, 0), index(indxRow, columnCount()), {Qt::DisplayRole});
        return;
    }

    replaceIt = m_servers.lower_bound(serverName);
    auto indxRow = std::distance(m_servers.begin(), replaceIt);
    beginInsertRows({}, indxRow, indxRow);
    m_servers[serverName] = serverAddress;
    endInsertRows();
}

QString ServerListModel::getIp(const QString &serverName) const
{
    auto targetIt = m_servers.find(serverName);
    if (targetIt != m_servers.end()) {
        return targetIt->second;
    }
    return {};
}
