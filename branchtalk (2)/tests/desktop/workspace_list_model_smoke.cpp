#include "workspace_list_model.hpp"

#include <QAbstractItemModel>
#include <QList>
#include <QObject>

#include <cstdlib>

int main()
{
    using branchtalk::desktop::WorkspaceListModel;

    WorkspaceListModel model;
    int loading_changes = 0;
    int empty_changes = 0;
    int selection_changes = 0;
    int selected_role_changes = 0;
    int unread_role_changes = 0;

    QObject::connect(&model,
                     &WorkspaceListModel::loadingChanged,
                     [&loading_changes]
                     { ++loading_changes; });
    QObject::connect(&model,
                     &WorkspaceListModel::emptyChanged,
                     [&empty_changes]
                     { ++empty_changes; });
    QObject::connect(&model,
                     &WorkspaceListModel::selectedWorkspaceIdChanged,
                     [&selection_changes]
                     { ++selection_changes; });
    QObject::connect(
        &model,
        &QAbstractItemModel::dataChanged,
        [&selected_role_changes, &unread_role_changes](const QModelIndex &,
                                                       const QModelIndex &,
                                                       const QList<int> &roles)
        {
            if (roles.contains(WorkspaceListModel::SelectedRole))
            {
                ++selected_role_changes;
            }
            if (roles.contains(WorkspaceListModel::UnreadCountRole))
            {
                ++unread_role_changes;
            }
        });

    const auto roles = model.roleNames();
    if (model.rowCount() != 0 || !model.empty() || model.loading() ||
        !model.selectedWorkspaceId().isEmpty() ||
        roles.value(WorkspaceListModel::StableIdRole) != "stableId" ||
        roles.value(WorkspaceListModel::NameRole) != "name" ||
        roles.value(WorkspaceListModel::SelectedRole) != "selected" ||
        roles.value(WorkspaceListModel::UnreadCountRole) != "unreadCount")
    {
        return EXIT_FAILURE;
    }

    model.setLoading(true);
    model.setLoading(true);
    model.setLoading(false);
    if (model.loading() || loading_changes != 2)
    {
        return EXIT_FAILURE;
    }

    if (!model.addWorkspace("workspace-alpha", "Alpha", 0) || model.empty() ||
        empty_changes != 1 ||
        !model.addWorkspace("workspace-beta", "Beta", 4) ||
        !model.addWorkspace("workspace-gamma", "Gamma", -3) ||
        model.addWorkspace("workspace-beta", "Duplicate", 1) ||
        model.addWorkspace({}, "Missing id", 1) || model.rowCount() != 3)
    {
        return EXIT_FAILURE;
    }

    const QModelIndex beta = model.index(1, 0);
    const QModelIndex gamma = model.index(2, 0);
    if (model.data(beta, WorkspaceListModel::StableIdRole).toString() !=
            "workspace-beta" ||
        model.data(beta, WorkspaceListModel::NameRole).toString() != "Beta" ||
        model.data(beta, WorkspaceListModel::UnreadCountRole).toInt() != 4 ||
        model.data(beta, WorkspaceListModel::SelectedRole).toBool() ||
        model.data(gamma, WorkspaceListModel::UnreadCountRole).toInt() != 0)
    {
        return EXIT_FAILURE;
    }

    if (!model.selectWorkspace("workspace-beta") ||
        model.selectedWorkspaceId() != "workspace-beta" ||
        !model.data(beta, WorkspaceListModel::SelectedRole).toBool() ||
        selection_changes != 1 || selected_role_changes != 1 ||
        !model.selectWorkspace("workspace-beta") || selection_changes != 1 ||
        selected_role_changes != 1 || model.selectWorkspace("workspace-missing"))
    {
        return EXIT_FAILURE;
    }

    if (!model.setUnreadCount("workspace-beta", 6) ||
        model.data(beta, WorkspaceListModel::UnreadCountRole).toInt() != 6 ||
        unread_role_changes != 1 ||
        model.setUnreadCount("workspace-missing", 2))
    {
        return EXIT_FAILURE;
    }

    if (!model.removeWorkspace("workspace-alpha") || model.rowCount() != 2 ||
        model.selectedWorkspaceId() != "workspace-beta")
    {
        return EXIT_FAILURE;
    }

    const QModelIndex shifted_beta = model.index(0, 0);
    if (model.data(shifted_beta, WorkspaceListModel::StableIdRole).toString() !=
            "workspace-beta" ||
        !model.data(shifted_beta, WorkspaceListModel::SelectedRole).toBool() ||
        selection_changes != 1)
    {
        return EXIT_FAILURE;
    }

    if (!model.removeWorkspace("workspace-beta") ||
        !model.selectedWorkspaceId().isEmpty() || selection_changes != 2 ||
        !model.removeWorkspace("workspace-gamma") || !model.empty() ||
        empty_changes != 2 || model.removeWorkspace("workspace-missing"))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
