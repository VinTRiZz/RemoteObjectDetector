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
        case C_address: return "Server address";
        case C_name:    return "Server name";
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

    if (role == Qt::DisplayRole) {
        auto serverIt = m_servers.begin();
        std::advance(serverIt, index.row());
        switch (index.column())
        {
        case C_address: return data(index, R_address);
        case C_name:    return data(index, R_name);
        }
        return {};
    }

    if (role == Qt::ToolTipRole) {
        switch (index.column())
        {
        case C_address: return "Server IP address";
        case C_name:    return "Server custom name";
        }
        return {};
    }

    if (role > Qt::UserRole) {
        auto serverIt = m_servers.begin();
        std::advance(serverIt, index.row());
        switch (role)
        {
        case R_address: return serverIt->getAddress().first;
        case R_name:    return serverIt->getName();
        }
    }

    return QVariant();
}

bool ServerListModel::addServer(const ServerConfiguration& conf)
{
    auto replaceIt = std::lower_bound(m_servers.begin(), m_servers.end(), conf.getHash(), [&conf](auto& sConf, auto v){
        return (sConf.getHash() == v);
    });
    if (replaceIt != m_servers.end() &&
        replaceIt->getHash() == conf.getHash()) {
        return false;
    }

    auto indxRow = std::distance(m_servers.begin(), replaceIt);
    beginInsertRows({}, indxRow, indxRow);
    m_servers.insert(conf);
    endInsertRows();
    return true;
}

std::optional<std::reference_wrapper<const ServerConfiguration> > ServerListModel::getServer(int serverRow) const
{
    if (serverRow >= m_servers.size()) {
        return {};
    }
    auto sPos = m_servers.begin();
    std::advance(sPos, serverRow);
    return *sPos;
}
