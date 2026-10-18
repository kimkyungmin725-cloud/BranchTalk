#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>
#include <QtQml/qqmlregistration.h>

namespace branchtalk::desktop
{

    class WorkspaceListModel : public QAbstractListModel
    {
        Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("WorkspaceListModel is provided by the application")

        Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
        Q_PROPERTY(bool empty READ empty NOTIFY emptyChanged)
        Q_PROPERTY(QString selectedWorkspaceId READ selectedWorkspaceId
                       NOTIFY selectedWorkspaceIdChanged)

    public:
        enum Role
        {
            StableIdRole = Qt::UserRole + 1,
            NameRole,
            SelectedRole,
            UnreadCountRole,
        };
        Q_ENUM(Role)

        struct Workspace
        {
            QString stable_id;
            QString name;
            int unread_count{0};
        };

        explicit WorkspaceListModel(QObject *parent = nullptr);

        [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex{}) const override;
        [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
        [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

        [[nodiscard]] bool loading() const noexcept;
        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] QString selectedWorkspaceId() const;

        void setLoading(bool loading);
        bool addWorkspace(QString stable_id, QString name, int unread_count = 0);
        bool removeWorkspace(const QString &stable_id);
        bool setUnreadCount(const QString &stable_id, int unread_count);
        Q_INVOKABLE bool selectWorkspace(const QString &stable_id);

    signals:
        void loadingChanged();
        void emptyChanged();
        void selectedWorkspaceIdChanged();

    private:
        [[nodiscard]] int indexOfStableId(const QString &stable_id) const noexcept;

        QVector<Workspace> workspaces_;
        QString selected_workspace_id_;
        bool loading_{false};
    };

} // namespace branchtalk::desktop
