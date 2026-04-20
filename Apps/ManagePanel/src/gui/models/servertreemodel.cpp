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
        case C_name:    return "Name";
        case C_host:    return "Host";
        case C_port:    return "Port";
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

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto serverIt = m_serversCache.begin();
        std::advance(serverIt, index.row());
        if (!serverIt->isValid()) {
            return {"[INVALID]"};
        }
        switch (index.column())
        {
        case C_host:    return data(index, R_host);
        case C_name:    return data(index, R_name);
        case C_port:    return data(index, R_port);
        }
        return {};
    }

    if (role == Qt::ToolTipRole) {
        switch (index.column())
        {
        case C_name:    return "Server custom name";
        case C_host:    return "Server IP address";
        case C_port:    return "Server connection listen port";
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
        case R_host:    return serverConf.getHost();
        case R_name:    return serverConf.getName();
        case R_port:    return serverConf.getPort();
        }
    }

    return QVariant();
}

bool ServerTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_serversCache.size() || index.row() < 0)
        return false;

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto serverIt = m_serversCache.begin();
        std::advance(serverIt, index.row());
        if (!serverIt->isValid()) {
            return false;
        }
        auto pServer = *serverIt;

        switch (index.column())
        {
        case C_host:    return setData(index, value, R_host);
        case C_name:    return setData(index, value, R_name);
        case C_port:    return setData(index, value, R_port);
        }
        return false;
    }

    if (role > Qt::UserRole) {
        auto serverIt = m_serversCache.begin();
        std::advance(serverIt, index.row());
        if (!serverIt->isValid()) {
            return {};
        }
        auto pServer = *serverIt;
        bool isDataChanged = false;
        switch (role)
        {
        case R_host:    isDataChanged = pServer->setHost(value.toString()); break;
        case R_port:    isDataChanged = pServer->setPort(value.toInt());    break;
        case R_name:    isDataChanged = pServer->setName(value.toString()); break;
        }
        if (isDataChanged) {
            emit dataChanged(index.siblingAtColumn(0), index.siblingAtColumn(columnCount() - 1), { Qt::DisplayRole });
        }
        return isDataChanged;
    }
    return false;
}

Qt::ItemFlags ServerTreeModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void ServerTreeModel::setServerRegistry(Web::ServerRegistry *pRegistry)
{
    beginResetModel();
    if (m_pServerRegistry) {
        disconnect(m_pServerRegistry, nullptr, this, nullptr);
    }
    m_pServerRegistry = pRegistry;
    if (m_pServerRegistry) {
        connect(m_pServerRegistry, &Web::ServerRegistry::initSucceed,
            this, [this](){
            beginResetModel();
            m_serversCache = m_pServerRegistry->getServers();
            endResetModel();
        });
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
