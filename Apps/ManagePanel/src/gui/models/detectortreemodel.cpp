#include "detectortreemodel.hpp"

#include <QDateTime>
#include <QColor>

#include <Components/Logger/Logger.h>

#include "client/detectorserver.hpp"
#include "client/detector.hpp"

static const int MODEL_COLUMN_COUNT {5};

DetectorTreeModel::DetectorTreeModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant DetectorTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        switch (section)
        {
        case C_id:              return "ID";
        case C_name:            return "Name";
        case C_location:        return "Location";
        case C_register_date:   return "Register date";
        case C_last_online:     return "Last online time";
        }
        return {};
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

int DetectorTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_detectorsCache.size();
}

int DetectorTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return MODEL_COLUMN_COUNT;
}

QVariant DetectorTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_detectorsCache.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column())
        {
        case C_id:              return data(index, R_id);
        case C_name:            return data(index, R_name);
        case C_location:        return data(index, R_location);
        case C_register_date:   return data(index, R_register_date);
        case C_last_online:     return data(index, R_last_online);
        }
        return {};
    }

    if (role == Qt::ToolTipRole) {
        switch (index.column())
        {
        case C_id:              return "Detector's id";
        case C_name:            return "Custom name of detector device";
        case C_location:        return "Detector location";
        case C_register_date:   return "Detector registration date";
        case C_last_online:     return "Detector last online date";
        }
        return {};
    }

    if (role == Qt::DecorationRole) {
        if (index.column() != 0) {
            return {};
        }
        auto isOnline = data(index, R_is_online).toBool();
        return (isOnline ? QColor(110, 240, 170) : QColor(240, 120, 150)); // TODO: Constants
    }

    if (role > Qt::UserRole) {
        auto pDetector = getDetector(index);
        if (!pDetector.isValid()) {
            return {};
        }
        auto& detectorConf = pDetector->getConfiguration();

        switch (role)
        {
        case R_id:              return QVariant::fromValue(detectorConf.system.id);
        case R_name:            return QString::fromStdString(detectorConf.info.name);
        case R_description:     return QString::fromStdString(detectorConf.info.description);
        case R_location:        return QString::fromStdString(detectorConf.info.location);
        case R_register_date:   return QDateTime::fromSecsSinceEpoch(detectorConf.system.registerDateUTC).toString("dd.MM.yyyy hh:mm");
        case R_last_online:     return (detectorConf.online.lastOnlineTimeUTC == 0 ? "ONLINE" : QDateTime::fromSecsSinceEpoch(detectorConf.online.lastOnlineTimeUTC).toString("dd.MM.yyyy hh:mm"));
        case R_is_online:       return detectorConf.online.lastOnlineTimeUTC == 0;
        }
    }

    return QVariant();
}

void DetectorTreeModel::setServer(const Web::ServerHandler &serv)
{
    beginResetModel();
    if (serv) {
        disconnect(serv.get(), nullptr, this, nullptr);
    }
    m_detectorsCache.clear();
    if (serv) {
        auto pDetServer = serv.cast_dynamic<Web::DetectorServer>();
        if (!pDetServer) {
            COMPLOG_WARNING("DetectorTreeModel: invalid server type (expected detector server)");
            endResetModel();
            return;
        }
        m_detectorsCache = pDetServer->getDetectors();
        connect(pDetServer, &Web::DetectorServer::detectorAdded,
                this, [this](const auto& detectorHdl){
                    // TODO: soft update, obviously
                    beginResetModel();
                    m_detectorsCache.push_back(detectorHdl);
                    std::sort(m_detectorsCache.begin(), m_detectorsCache.end());
                    endResetModel();
                });

        connect(pDetServer, &Web::DetectorServer::detectorAboutToRemove,
                this, [this](const auto& detectorHdl){
                    // TODO: soft update, obviously
                    beginResetModel();
                    auto targetIt = std::find(m_detectorsCache.begin(), m_detectorsCache.end(), detectorHdl);
                    m_detectorsCache.erase(targetIt);
                    endResetModel();
                });
    }
    m_currentServer = serv;
    endResetModel();
}

Web::DetectorHandler DetectorTreeModel::getDetector(const QModelIndex &idx) const
{
    if (idx.row() >= rowCount() || idx.row() < 0) {
        return {};
    }
    auto detectorIt = m_detectorsCache.begin();
    std::advance(detectorIt, idx.row());
    return *detectorIt;
}
