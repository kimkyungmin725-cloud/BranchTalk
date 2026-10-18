#include "workspace_list_model.hpp"

#include <algorithm>
#include <utility>

namespace branchtalk::desktop
{

    WorkspaceListModel::WorkspaceListModel(QObject *parent) : QAbstractListModel{parent} {}

    int WorkspaceListModel::rowCount(const QModelIndex &parent) const
    {
        return parent.isValid() ? 0 : static_cast<int>(workspaces_.size());
    }

    QVariant WorkspaceListModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid() || index.parent().isValid() || index.column() != 0 ||
            index.row() < 0 || index.row() >= rowCount())
        {
            return {};
        }

        const auto &workspace = workspaces_.at(index.row());
        switch (role)
        {
        case StableIdRole:
            return workspace.stable_id;
        case NameRole:
            return workspace.name;
        case SelectedRole:
            return workspace.stable_id == selected_workspace_id_;
        case UnreadCountRole:
            return workspace.unread_count;
        default:
            return {};
        }
    }

    QHash<int, QByteArray> WorkspaceListModel::roleNames() const
    {
        return {
            {StableIdRole, "stableId"},
            {NameRole, "name"},
            {SelectedRole, "selected"},
            {UnreadCountRole, "unreadCount"},
        };
    }

    bool WorkspaceListModel::loading() const noexcept
    {
        return loading_;
    }

    bool WorkspaceListModel::empty() const noexcept
    {
        return workspaces_.isEmpty();
    }

    QString WorkspaceListModel::selectedWorkspaceId() const
    {
        return selected_workspace_id_;
    }

    void WorkspaceListModel::setLoading(bool loading)
    {
        if (loading_ == loading)
        {
            return;
        }

        loading_ = loading;
        emit loadingChanged();
    }

    bool WorkspaceListModel::addWorkspace(QString stable_id,
                                          QString name,
                                          int unread_count)
    {
        if (stable_id.isEmpty() || name.isEmpty() || indexOfStableId(stable_id) >= 0)
        {
            return false;
        }

        const bool was_empty = empty();
        const int row = rowCount();
        beginInsertRows(QModelIndex{}, row, row);
        workspaces_.append(Workspace{
            std::move(stable_id),
            std::move(name),
            std::max(0, unread_count),
        });
        endInsertRows();

        if (was_empty)
        {
            emit emptyChanged();
        }
        return true;
    }

    bool WorkspaceListModel::removeWorkspace(const QString &stable_id)
    {
        const int row = indexOfStableId(stable_id);
        if (row < 0)
        {
            return false;
        }

        const bool removed_selection = stable_id == selected_workspace_id_;
        const bool becomes_empty = rowCount() == 1;
        beginRemoveRows(QModelIndex{}, row, row);
        workspaces_.removeAt(row);
        endRemoveRows();

        if (removed_selection)
        {
            selected_workspace_id_.clear();
            emit selectedWorkspaceIdChanged();
        }
        if (becomes_empty)
        {
            emit emptyChanged();
        }
        return true;
    }

    bool WorkspaceListModel::setUnreadCount(const QString &stable_id, int unread_count)
    {
        const int row = indexOfStableId(stable_id);
        if (row < 0)
        {
            return false;
        }

        auto &workspace = workspaces_[row];
        const int normalized_count = std::max(0, unread_count);
        if (workspace.unread_count == normalized_count)
        {
            return true;
        }

        workspace.unread_count = normalized_count;
        const QModelIndex changed_index = index(row, 0);
        emit dataChanged(changed_index, changed_index, {UnreadCountRole});
        return true;
    }

    bool WorkspaceListModel::selectWorkspace(const QString &stable_id)
    {
        const int selected_row = indexOfStableId(stable_id);
        if (selected_row < 0)
        {
            return false;
        }
        if (selected_workspace_id_ == stable_id)
        {
            return true;
        }

        const int previous_row = indexOfStableId(selected_workspace_id_);
        selected_workspace_id_ = stable_id;

        if (previous_row >= 0)
        {
            const QModelIndex previous_index = index(previous_row, 0);
            emit dataChanged(previous_index, previous_index, {SelectedRole});
        }
        const QModelIndex selected_index = index(selected_row, 0);
        emit dataChanged(selected_index, selected_index, {SelectedRole});
        emit selectedWorkspaceIdChanged();
        return true;
    }

    int WorkspaceListModel::indexOfStableId(const QString &stable_id) const noexcept
    {
        const int count = rowCount();
        for (int row = 0; row < count; ++row)
        {
            if (workspaces_.at(row).stable_id == stable_id)
            {
                return row;
            }
        }
        return -1;
    }

} // namespace branchtalk::desktop
