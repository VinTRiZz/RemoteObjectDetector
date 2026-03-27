#pragma once

#include <QAbstractTableModel>

#include <ROD/DetectorConfiguration.h>

class DetectorInfoManager;

class DetectorListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DetectorListModel(QObject *parent = nullptr);

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
    };

    // QAbstractTableModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setDetectorInfoManager(DetectorInfoManager* pManager);
    void updateDetectorList();

private:
    DetectorInfoManager* m_pDetectorInfoManager {nullptr};
    std::vector<DataObjects::DetectorConfiguration> m_detectors;

    void addDetector(DataObjects::id_t detectorId);
};

