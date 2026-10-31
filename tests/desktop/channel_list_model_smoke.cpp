#include "channel_list_model.hpp"

#include <QAbstractItemModel>
#include <QList>
#include <QObject>
#include <QVector>

#include <cstdlib>
#include <utility>

namespace
{
    struct DataChange
    {
        int first_row;
        int last_row;
        QList<int> roles;
    };
} // namespace

int main()
{
    using branchtalk::desktop::ChannelListModel;

    QVector<ChannelListModel::Channel> channels{
        {"channel-general", "general", ChannelListModel::TextChannel, 3, false},
        {"channel-product", "product", ChannelListModel::TextChannel, 0, false},
        {"channel-support", "support", ChannelListModel::TextChannel, 7, true},
        {"channel-daily", "daily-standup", ChannelListModel::VoiceChannel, 1, false},
    };
    ChannelListModel model{std::move(channels)};

    int model_resets = 0;
    int selection_changes = 0;
    int search_changes = 0;
    int match_count_changes = 0;
    QVector<DataChange> data_changes;

    QObject::connect(&model,
                     &QAbstractItemModel::modelReset,
                     [&model_resets]
                     { ++model_resets; });
    QObject::connect(&model,
                     &ChannelListModel::selectedChannelChanged,
                     [&selection_changes]
                     { ++selection_changes; });
    QObject::connect(&model,
                     &ChannelListModel::searchQueryChanged,
                     [&search_changes]
                     { ++search_changes; });
    QObject::connect(&model,
                     &ChannelListModel::matchingChannelCountChanged,
                     [&match_count_changes]
                     { ++match_count_changes; });
    QObject::connect(&model,
                     &QAbstractItemModel::dataChanged,
                     [&data_changes](const QModelIndex &first,
                                     const QModelIndex &last,
                                     const QList<int> &roles)
                     {
                         data_changes.append({first.row(), last.row(), roles});
                     });

    const auto roles = model.roleNames();
    const QModelIndex general = model.index(0, 0);
    const QModelIndex support = model.index(2, 0);
    const QModelIndex daily = model.index(3, 0);
    if (model.rowCount() != 4 || model.rowCount(model.index(0, 0)) != 0 ||
        model.matchingChannelCount() != 4 || !model.searchQuery().isEmpty() ||
        !model.selectedChannelId().isEmpty() ||
        roles.value(ChannelListModel::StableIdRole) != "stableId" ||
        roles.value(ChannelListModel::NameRole) != "name" ||
        roles.value(ChannelListModel::KindRole) != "kind" ||
        roles.value(ChannelListModel::SelectedRole) != "selected" ||
        roles.value(ChannelListModel::UnreadCountRole) != "unreadCount" ||
        roles.value(ChannelListModel::MutedRole) != "muted" ||
        roles.value(ChannelListModel::MatchesSearchRole) != "matchesSearch" ||
        model.data(general, ChannelListModel::UnreadCountRole).toInt() != 3 ||
        model.data(support, ChannelListModel::MutedRole).toBool() != true ||
        model.data(daily, ChannelListModel::KindRole).toInt() !=
            ChannelListModel::VoiceChannel)
    {
        return EXIT_FAILURE;
    }

    if (!model.selectChannel("channel-general") ||
        model.selectedChannelId() != "channel-general" ||
        model.selectedChannelName() != "general" ||
        model.selectedChannelKind() != ChannelListModel::TextChannel ||
        !model.data(general, ChannelListModel::SelectedRole).toBool() ||
        selection_changes != 1 || data_changes.size() != 1 ||
        data_changes.constFirst().first_row != 0 ||
        data_changes.constFirst().roles != QList<int>{ChannelListModel::SelectedRole} ||
        !model.selectChannel("channel-general") || selection_changes != 1 ||
        model.selectChannel("channel-missing"))
    {
        return EXIT_FAILURE;
    }

    data_changes.clear();
    if (!model.setUnreadCount("channel-product", 4) ||
        !model.setUnreadCount("channel-product", 4) ||
        !model.setMuted("channel-product", true) ||
        !model.setMuted("channel-product", true) ||
        model.setUnreadCount("channel-missing", 1) ||
        model.setMuted("channel-missing", true) || data_changes.size() != 2 ||
        data_changes.at(0).first_row != 1 || data_changes.at(0).last_row != 1 ||
        data_changes.at(0).roles != QList<int>{ChannelListModel::UnreadCountRole} ||
        data_changes.at(1).first_row != 1 ||
        data_changes.at(1).roles != QList<int>{ChannelListModel::MutedRole})
    {
        return EXIT_FAILURE;
    }

    model.setSearchQuery("DAILY");
    if (model.searchQuery() != "DAILY" || model.matchingChannelCount() != 1 ||
        search_changes != 1 || match_count_changes != 1 || data_changes.size() != 3 ||
        data_changes.at(2).first_row != 0 || data_changes.at(2).last_row != 3 ||
        data_changes.at(2).roles != QList<int>{ChannelListModel::MatchesSearchRole} ||
        model.data(general, ChannelListModel::MatchesSearchRole).toBool() ||
        !model.data(daily, ChannelListModel::MatchesSearchRole).toBool())
    {
        return EXIT_FAILURE;
    }

    model.setSearchQuery("DAILY");
    model.setSearchQuery({});
    if (model.matchingChannelCount() != 4 || search_changes != 2 ||
        match_count_changes != 2 || data_changes.size() != 4)
    {
        return EXIT_FAILURE;
    }

    data_changes.clear();
    if (!model.selectChannel("channel-daily") ||
        model.selectedChannelId() != "channel-daily" ||
        model.selectedChannelName() != "daily-standup" ||
        model.selectedChannelKind() != ChannelListModel::VoiceChannel ||
        model.data(general, ChannelListModel::SelectedRole).toBool() ||
        !model.data(daily, ChannelListModel::SelectedRole).toBool() ||
        selection_changes != 2 || data_changes.size() != 2 ||
        data_changes.at(0).first_row != 0 ||
        data_changes.at(0).roles != QList<int>{ChannelListModel::SelectedRole} ||
        data_changes.at(1).first_row != 3 ||
        data_changes.at(1).roles != QList<int>{ChannelListModel::SelectedRole} ||
        model_resets != 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}