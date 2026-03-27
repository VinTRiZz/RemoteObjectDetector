#include "detectorlistmodel.hpp"

#include <QDateTime>

#include <Components/Logger/Logger.h>

#include "client/detectorinfomanager.hpp"

DetectorListModel::DetectorListModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QVariant DetectorListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        switch (section)
        {
        case C_id:      return "ID";
        case C_name:    return "Название";
        }
        return {};
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

int DetectorListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_detectors.size();
}

int DetectorListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant DetectorListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_detectors.size() || index.row() < 0)
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

    if (role > Qt::UserRole) {
        auto detectorIt = m_detectors.begin();
        std::advance(detectorIt, index.row());

        switch (role)
        {
        case R_id:              return QVariant::fromValue(detectorIt->system.id);
        case R_name:            return QString::fromStdString(detectorIt->info.name);
        case R_description:     return QString::fromStdString(detectorIt->info.description);
        case R_location:        return QString::fromStdString(detectorIt->info.location);
        case R_register_date:   return QDateTime::fromSecsSinceEpoch(detectorIt->system.registerDateUTC).toString("dd.MM.yyyy hh:mm");
        case R_last_online:     return (detectorIt->online.lastOnlineTimeUTC == 0 ? "ONLINE" : QDateTime::fromSecsSinceEpoch(detectorIt->online.lastOnlineTimeUTC).toString("dd.MM.yyyy hh:mm"));
        }
    }

    return QVariant();
}

void DetectorListModel::setDetectorInfoManager(DetectorInfoManager *pManager)
{
    if (nullptr == pManager) {
        COMPLOG_WARNING("[DetectorListModel] Invalid detector info manager");
        return;
    }
    m_pDetectorInfoManager = pManager;
    connect(m_pDetectorInfoManager, &DetectorInfoManager::responseDetectorInfo,
            this, [this](bool isOk, const auto& detectorInfo){
        auto insertRow = m_detectors.empty() ? 0 : m_detectors.size() - 1;
        beginInsertRows({}, insertRow, insertRow);
        m_detectors.push_back(detectorInfo);
        endInsertRows();
    });

    connect(m_pDetectorInfoManager, &DetectorInfoManager::responseDetectorList,
            this, [this](bool isOk, const auto& idVect){
        m_detectors.clear();
        beginResetModel();
        for (auto id : idVect) {
            addDetector(id);
        }
        endResetModel();
    });
}

void DetectorListModel::updateDetectorList()
{
    if (nullptr == m_pDetectorInfoManager) {
        COMPLOG_WARNING("[DetectorListModel] Detector info manager not set");
        return;
    }

    m_pDetectorInfoManager->requestDetectorList();
}

void DetectorListModel::addDetector(DataObjects::id_t detectorId)
{
    if (nullptr == m_pDetectorInfoManager) {
        COMPLOG_WARNING("[DetectorListModel] Detector info manager not set");
        return;
    }

    m_pDetectorInfoManager->requestDetectorInfo(detectorId);
}