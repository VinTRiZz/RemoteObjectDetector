#pragma once

#include <QAbstractTableModel>

#include "client/handlers.hpp"

#include <set>

class DetectorInfoManager;

class DetectorTreeModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DetectorTreeModel(QObject *parent = nullptr);

    enum Columns : int {
        C_id = 0,
        C_name,
        C_location,
        C_register_date,
        C_last_online,
    };

    enum Roles : int {
        R_id = Qt::UserRole + 1,
        R_name,
        R_description,
        R_location,
        R_register_date,
        R_last_online,
        R_is_online,
    };

    // QAbstractTableModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setServer(const Web::ServerHandler& serv);

private:
    std::set<Web::DetectorHandler>  m_detectorsCache;
    Web::ServerHandler              m_currentServer;
};

