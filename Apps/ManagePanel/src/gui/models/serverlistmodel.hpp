#pragma once

#include <QAbstractTableModel>

#include "common/serverconfiguration.hpp"

#include <set>
#include <optional>

class ServerListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ServerListModel(QObject *parent = nullptr);

    enum Columns : int {
        C_address = 0,
        C_name,
    };

    enum Roles : int {
        R_address = Qt::UserRole + 1,
        R_name
    };

    // QAbstractTableModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool addServer(const ServerConfiguration &conf);
    std::optional<std::reference_wrapper<const ServerConfiguration> > getServer(int serverRow) const;

private:
    std::set<ServerConfiguration> m_servers;
};

