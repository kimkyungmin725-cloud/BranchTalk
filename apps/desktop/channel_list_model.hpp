#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>
#include <QtQml/qqmlregistration.h>

namespace branchtalk::desktop
{

    class ChannelListModel : public QAbstractListModel
    {
        Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("ChannelListModel is provided by the application")

        Q_PROPERTY(QString searchQuery READ searchQuery NOTIFY searchQueryChanged)
        Q_PROPERTY(int matchingChannelCount READ matchingChannelCount
                       NOTIFY matchingChannelCountChanged)
        Q_PROPERTY(QString selectedChannelId READ selectedChannelId
                       NOTIFY selectedChannelChanged)
        Q_PROPERTY(QString selectedChannelName READ selectedChannelName
                       NOTIFY selectedChannelChanged)
        Q_PROPERTY(ChannelKind selectedChannelKind READ selectedChannelKind
                       NOTIFY selectedChannelChanged)

    public:
        enum ChannelKind
        {
            TextChannel,
            VoiceChannel,
        };
        Q_ENUM(ChannelKind)

        enum Role
        {
            StableIdRole = Qt::UserRole + 1,
            NameRole,
            KindRole,
            SelectedRole,
            UnreadCountRole,
            MutedRole,
            MatchesSearchRole,
        };
        Q_ENUM(Role)

        struct Channel
        {
            QString stable_id;
            QString name;
            ChannelKind kind{TextChannel};
            int unread_count{0};
            bool muted{false};
        };

        explicit ChannelListModel(QObject *parent = nullptr);
        explicit ChannelListModel(QVector<Channel> channels, QObject *parent = nullptr);

        [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex{}) const override;
        [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
        [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

        [[nodiscard]] QString searchQuery() const;
        [[nodiscard]] int matchingChannelCount() const noexcept;
        [[nodiscard]] QString selectedChannelId() const;
        [[nodiscard]] QString selectedChannelName() const;
        [[nodiscard]] ChannelKind selectedChannelKind() const noexcept;

        Q_INVOKABLE void setSearchQuery(QString search_query);
        Q_INVOKABLE bool selectChannel(const QString &stable_id);
        bool setUnreadCount(const QString &stable_id, int unread_count);
        bool setMuted(const QString &stable_id, bool muted);

    signals:
        void searchQueryChanged();
        void matchingChannelCountChanged();
        void selectedChannelChanged();

    private:
        [[nodiscard]] int indexOfStableId(const QString &stable_id) const noexcept;
        [[nodiscard]] bool matchesSearch(const Channel &channel) const;

        QVector<Channel> channels_;
        QString search_query_;
        QString selected_channel_id_;
    };
} // namespace branchtalk::desktop