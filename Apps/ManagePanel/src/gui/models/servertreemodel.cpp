#include "servertreemodel.hpp"

#include "client/server.hpp"

ServerTreeModel::ServerTreeModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant ServerTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int ServerTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_serversCache.size();
}

int ServerTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant ServerTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_serversCache.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        auto serverIt = m_serversCache.begin();
        std::advance(serverIt, index.row());
        if (!serverIt->isValid()) {
            return {"[INVALID]"};
        }
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
        auto serverIt = m_serversCache.begin();
        std::advance(serverIt, index.row());
        if (!serverIt->isValid()) {
            return {};
        }
        auto& serverConf = (*serverIt)->getConfiguration();
        switch (role)
        {
        case R_address: return serverConf.getAddress();
        case R_name:    return serverConf.getName();
        }
    }

    return QVariant();
}

void ServerTreeModel::setServerRegistry(Web::ServerRegistry *pRegistry)
{
    beginResetModel();
    if (m_pServerRegistry) {
        disconnect(m_pServerRegistry, nullptr, this, nullptr);
    }
    m_pServerRegistry = pRegistry;
    if (m_pServerRegistry) {
        connect(m_pServerRegistry, &Web::ServerRegistry::serverAdded,
                this, [this](const auto& serverHdl){
                    // TODO: soft update, obviously
                    beginResetModel();
                    m_serversCache.insert(serverHdl);
                    endResetModel();
                });

        connect(m_pServerRegistry, &Web::ServerRegistry::serverAboutToRemove,
                this, [this](const auto& serverHdl){
                    // TODO: soft update, obviously
                    beginResetModel();
                    m_serversCache.erase(serverHdl);
                    endResetModel();
                });
        m_serversCache = m_pServerRegistry->getServers();
    }
    endResetModel();
}

Web::ServerRegistry *ServerTreeModel::getServerRegistry() const
{
    return m_pServerRegistry;
}

Web::ServerHandler ServerTreeModel::getServer(int serverRow) const
{
    if (serverRow >= m_serversCache.size()) {
        return {};
    }
    auto sPos = m_serversCache.begin();
    std::advance(sPos, serverRow);
    return *sPos;
}
