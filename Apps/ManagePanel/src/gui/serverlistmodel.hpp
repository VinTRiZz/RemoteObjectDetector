#pragma once

#include <QAbstractTableModel>

#include <map>

class ServerListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ServerListModel(QObject *parent = nullptr);

    enum Roles : int {
        ServerIp = Qt::UserRole + 1,
    };

    // QAbstractTableModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addServer(const QString& serverName, const QString& serverAddress);
    QString getIp(const QString& serverName) const;

private:
    std::map<QString, QString> m_servers;
};

