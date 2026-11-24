#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>
#include <QtQml/qqmlregistration.h>

namespace branchtalk::desktop {

class MessageListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("MessageListModel is provided by the application")

public:
    enum MessageStatus {
        Pending,
        Sent,
        Failed,
    };
    Q_ENUM(MessageStatus)

    enum Role {
        StableIdRole = Qt::UserRole + 1,
        AuthorIdRole,
        AuthorNameRole,
        TimestampRole,
        BodyRole,
        StatusRole,
        GroupedWithPreviousRole,
    };
    Q_ENUM(Role)

    struct Message {
        QString stable_id;
        QString author_id;
        QString author_name;
        QString timestamp;
        QString body;
        MessageStatus status{Sent};
    };

    explicit MessageListModel(QObject* parent = nullptr);
    explicit MessageListModel(QVector<Message> messages, QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex{}) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    bool updateMessage(Message message);

private:
    [[nodiscard]] int indexOfStableId(const QString& stable_id) const noexcept;
    [[nodiscard]] bool groupedWithPrevious(int row) const noexcept;
    [[nodiscard]] static bool isValid(const Message& message) noexcept;

    QVector<Message> messages_;
};

}  // namespace branchtalk::desktop
