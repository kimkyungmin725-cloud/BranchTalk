#include "channel_list_model.hpp"

#include <QSet>

#include <algorithm>
#include <utility>

namespace branchtalk::desktop
{
    ChannelListModel::ChannelListModel(QObject *parent) : QAbstractListModel{parent} {}

    ChannelListModel::ChannelListModel(QVector<Channel> channels, QObject *parent)
        : QAbstractListModel{parent}
    {
        QSet<QString> stable_ids;
        channels_.reserve(channels.size());
        for (auto &channel : channels)
        {
            if (channel.stable_id.isEmpty() || channel.name.isEmpty() ||
                stable_ids.contains(channel.stable_id) ||
                (channel.kind != TextChannel && channel.kind != VoiceChannel))
            {
                continue;
            }

            stable_ids.insert(channel.stable_id);
            channel.unread_count = std::max(0, channel.unread_count);
            channels_.append(std::move(channel));
        }
    }

    int ChannelListModel::rowCount(const QModelIndex &parent) const
    {
        return parent.isValid() ? 0 : static_cast<int>(channels_.size());
    }

    QVariant ChannelListModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid() || index.parent().isValid() || index.column() != 0 ||
            index.row() < 0 || index.row() >= rowCount())
        {
            return {};
        }

        const auto &channel = channels_.at(index.row());
        switch (role)
        {
        case StableIdRole:
            return channel.stable_id;
        case NameRole:
            return channel.name;
        case KindRole:
            return static_cast<int>(channel.kind);
        case SelectedRole:
            return channel.stable_id == selected_channel_id_;
        case UnreadCountRole:
            return channel.unread_count;
        case MutedRole:
            return channel.muted;
        case MatchesSearchRole:
            return matchesSearch(channel);
        default:
            return {};
        }
    }

    QHash<int, QByteArray> ChannelListModel::roleNames() const
    {
        return {
            {StableIdRole, "stableId"},
            {NameRole, "name"},
            {KindRole, "kind"},
            {SelectedRole, "selected"},
            {UnreadCountRole, "unreadCount"},
            {MutedRole, "muted"},
            {MatchesSearchRole, "matchesSearch"},
        };
    }

    QString ChannelListModel::searchQuery() const
    {
        return search_query_;
    }

    int ChannelListModel::matchingChannelCount() const noexcept
    {
        return static_cast<int>(
            std::count_if(channels_.cbegin(), channels_.cend(), [this](const auto &channel)
                          { return matchesSearch(channel); }));
    }

    QString ChannelListModel::selectedChannelId() const
    {
        return selected_channel_id_;
    }

    QString ChannelListModel::selectedChannelName() const
    {
        const int row = indexOfStableId(selected_channel_id_);
        return row >= 0 ? channels_.at(row).name : QString();
    }

    ChannelListModel::ChannelKind ChannelListModel::selectedChannelKind() const noexcept
    {
        const int row = indexOfStableId(selected_channel_id_);
        return row >= 0 ? channels_.at(row).kind : TextChannel;
    }

    void ChannelListModel::setSearchQuery(QString search_query)
    {
        search_query = search_query.trimmed();
        if (search_query_ == search_query)
        {
            return;
        }

        const int previous_match_count = matchingChannelCount();
        search_query_ = std::move(search_query);
        const int current_match_count = matchingChannelCount();

        emit searchQueryChanged();
        if (rowCount() > 0)
        {
            emit dataChanged(index(0, 0), index(rowCount() - 1, 0), {MatchesSearchRole});
        }
        if (previous_match_count != current_match_count)
        {
            emit matchingChannelCountChanged();
        }
    }

    bool ChannelListModel::selectChannel(const QString &stable_id)
    {
        const int selected_row = indexOfStableId(stable_id);
        if (selected_row < 0)
        {
            return false;
        }
        if (selected_channel_id_ == stable_id)
        {
            return true;
        }

        const int previous_row = indexOfStableId(selected_channel_id_);
        selected_channel_id_ = stable_id;

        if (previous_row >= 0)
        {
            const QModelIndex previous_index = index(previous_row, 0);
            emit dataChanged(previous_index, previous_index, {SelectedRole});
        }
        const QModelIndex selected_index = index(selected_row, 0);
        emit dataChanged(selected_index, selected_index, {SelectedRole});
        emit selectedChannelChanged();
        return true;
    }

    bool ChannelListModel::setUnreadCount(const QString &stable_id, int unread_count)
    {
        const int row = indexOfStableId(stable_id);
        if (row < 0)
        {
            return false;
        }

        auto &channel = channels_[row];
        const int normalized_count = std::max(0, unread_count);
        if (channel.unread_count == normalized_count)
        {
            return true;
        }

        channel.unread_count = normalized_count;
        const QModelIndex changed_index = index(row, 0);
        emit dataChanged(changed_index, changed_index, {UnreadCountRole});
        return true;
    }

    bool ChannelListModel::setMuted(const QString &stable_id, bool muted)
    {
        const int row = indexOfStableId(stable_id);
        if (row < 0)
        {
            return false;
        }

        auto &channel = channels_[row];
        if (channel.muted == muted)
        {
            return true;
        }

        channel.muted = muted;
        const QModelIndex changed_index = index(row, 0);
        emit dataChanged(changed_index, changed_index, {MutedRole});
        return true;
    }

    int ChannelListModel::indexOfStableId(const QString &stable_id) const noexcept
    {
        const int count = rowCount();
        for (int row = 0; row < count; row++)
        {
            if (channels_.at(row).stable_id == stable_id)
            {
                return row;
            }
        }
        return -1;
    }

    bool ChannelListModel::matchesSearch(const Channel &channel) const
    {
        return search_query_.isEmpty() || channel.name.contains(search_query_, Qt::CaseInsensitive);
    }
} // namespace branchtalk::desktop