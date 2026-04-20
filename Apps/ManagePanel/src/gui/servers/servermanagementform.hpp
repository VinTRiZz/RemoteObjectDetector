#pragma once

#include <QWidget>

class ServerTreeModel;
namespace Web {
class ServerRegistry;
class ServerHandler;
}

// Qt
class QIdentityProxyModel;

namespace Ui {
class ServerManagementForm;
}

class ServerManagementForm : public QWidget
{
    Q_OBJECT

public:
    explicit ServerManagementForm(QWidget *parent = nullptr);
    ~ServerManagementForm();

    // For easier usage
    void setSourceServerModel(ServerTreeModel* pSourceModel);

private:
    Ui::ServerManagementForm *ui;

    Web::ServerRegistry* m_pCurrentServerRegistry {nullptr};

    ServerTreeModel*        m_pServerTreeModel {nullptr};
    QIdentityProxyModel*    m_pServerTreeIdentityModel {nullptr};

    void processSelectedServer(const Web::ServerHandler& serv);
};

