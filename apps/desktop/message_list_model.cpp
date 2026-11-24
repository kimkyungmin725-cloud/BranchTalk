#include "message_list_model.hpp"

#include <QList>
#include <QSet>

#include <utility>

namespace branchtalk::desktop {

MessageListModel::MessageListModel(QObject* parent) : QAbstractListModel{parent} {}

MessageListModel::MessageListModel(QVector<Message> messages, QObject* parent)
    : QAbstractListModel{parent} {
    QSet<QString> stable_ids;
    messages_.reserve(messages.size());
    for (auto& message : messages) {
        if (!isValid(message) || stable_ids.contains(message.stable_id)) {
            continue;
        }

        stable_ids.insert(message.stable_id);
        messages_.append(std::move(message));
    }
}

int MessageListModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(messages_.size());
}

QVariant MessageListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.parent().isValid() || index.column() != 0 ||
        index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const auto& message = messages_.at(index.row());
    switch (role) {
        case StableIdRole:
            return message.stable_id;
        case AuthorIdRole:
            return message.author_id;
        case AuthorNameRole:
            return message.author_name;
        case TimestampRole:
            return message.timestamp;
        case BodyRole:
            return message.body;
        case StatusRole:
            return static_cast<int>(message.status);
        case GroupedWithPreviousRole:
            return groupedWithPrevious(index.row());
        default:
            return {};
    }
}

QHash<int, QByteArray> MessageListModel::roleNames() const {
    return {
        {StableIdRole, "stableId"},
        {AuthorIdRole, "authorId"},
        {AuthorNameRole, "authorName"},
        {TimestampRole, "timestamp"},
        {BodyRole, "body"},
        {StatusRole, "status"},
        {GroupedWithPreviousRole, "groupedWithPrevious"},
    };
}

bool MessageListModel::updateMessage(Message message) {
    if (!isValid(message)) {
        return false;
    }

    const int row = indexOfStableId(message.stable_id);
    if (row < 0) {
        return false;
    }

    const bool was_grouped = groupedWithPrevious(row);
    const bool next_was_grouped = groupedWithPrevious(row + 1);
    const auto& current = messages_.at(row);
    QList<int> changed_roles;
    if (current.author_id != message.author_id) {
        changed_roles.append(AuthorIdRole);
    }
    if (current.author_name != message.author_name) {
        changed_roles.append(AuthorNameRole);
    }
    if (current.timestamp != message.timestamp) {
        changed_roles.append(TimestampRole);
    }
    if (current.body != message.body) {
        changed_roles.append(BodyRole);
    }
    if (current.status != message.status) {
        changed_roles.append(StatusRole);
    }
    if (changed_roles.isEmpty()) {
        return true;
    }

    messages_[row] = std::move(message);

    if (was_grouped != groupedWithPrevious(row)) {
        changed_roles.append(GroupedWithPreviousRole);
    }
    const QModelIndex changed_index = index(row, 0);
    emit dataChanged(changed_index, changed_index, changed_roles);

    if (row + 1 < rowCount() && next_was_grouped != groupedWithPrevious(row + 1)) {
        const QModelIndex next_index = index(row + 1, 0);
        emit dataChanged(next_index, next_index, {GroupedWithPreviousRole});
    }
    return true;
}

int MessageListModel::indexOfStableId(const QString& stable_id) const noexcept {
    const int count = rowCount();
    for (int row = 0; row < count; ++row) {
        if (messages_.at(row).stable_id == stable_id) {
            return row;
        }
    }
    return -1;
}

bool MessageListModel::groupedWithPrevious(int row) const noexcept {
    return row > 0 && row < rowCount() &&
           messages_.at(row - 1).author_id == messages_.at(row).author_id;
}

bool MessageListModel::isValid(const Message& message) noexcept {
    return !message.stable_id.isEmpty() && !message.author_id.isEmpty() &&
           !message.author_name.isEmpty() && !message.timestamp.isEmpty() &&
           !message.body.isEmpty() &&
           (message.status == Pending || message.status == Sent ||
            message.status == Failed);
}

}  // namespace branchtalk::desktop
