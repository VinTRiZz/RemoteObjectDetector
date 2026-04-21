#pragma once

#include <QAbstractTableModel>

#include "client/serverregistry.hpp"

#include <set>

class ServerTreeModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ServerTreeModel(QObject *parent = nullptr);

    enum Columns : int {
        C_name = 0,
        C_host,
        C_port,
    };

    enum Roles : int {
        R_name = Qt::UserRole + 1,
        R_host,
        R_port,
    };

    // QAbstractTableModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setServerRegistry(Web::ServerRegistry* pRegistry);
    Web::ServerRegistry* getServerRegistry() const;

    Web::ServerHandler getServer(const QModelIndex& idx) const;

private:
    std::set<Web::ServerHandler>    m_serversCache;
    Web::ServerRegistry*            m_pServerRegistry {nullptr};
};

