#include "message_list_model.hpp"

#include <QAbstractItemModel>
#include <QList>
#include <QObject>
#include <QVector>

#include <cstdlib>
#include <utility>

namespace {

struct DataChange {
    int first_row;
    int last_row;
    QList<int> roles;
};

}  // namespace

int main() {
    using branchtalk::desktop::MessageListModel;

    QVector<MessageListModel::Message> messages{
        {"message-1", "user-mina", "Mina", "09:00", "First", MessageListModel::Sent},
        {"message-2", "user-mina", "Mina", "09:01", "Second", MessageListModel::Sent},
        {"message-3", "user-alex", "Alex", "09:02", "Third", MessageListModel::Pending},
        {"message-4", "user-alex", "Alex", "09:03", "Fourth", MessageListModel::Failed},
        {"message-4", "user-alex", "Alex", "09:04", "Duplicate", MessageListModel::Sent},
        {"", "user-alex", "Alex", "09:05", "Missing id", MessageListModel::Sent},
    };
    MessageListModel model{std::move(messages)};

    int model_resets = 0;
    QVector<DataChange> data_changes;
    QObject::connect(&model,
                     &QAbstractItemModel::modelReset,
                     [&model_resets] { ++model_resets; });
    QObject::connect(
        &model,
        &QAbstractItemModel::dataChanged,
        [&data_changes](const QModelIndex& first,
                        const QModelIndex& last,
                        const QList<int>& roles) {
            data_changes.append({first.row(), last.row(), roles});
        });

    const auto roles = model.roleNames();
    const QModelIndex first = model.index(0, 0);
    const QModelIndex second = model.index(1, 0);
    const QModelIndex third = model.index(2, 0);
    if (model.rowCount() != 4 || model.rowCount(first) != 0 ||
        roles.value(MessageListModel::StableIdRole) != "stableId" ||
        roles.value(MessageListModel::AuthorIdRole) != "authorId" ||
        roles.value(MessageListModel::AuthorNameRole) != "authorName" ||
        roles.value(MessageListModel::TimestampRole) != "timestamp" ||
        roles.value(MessageListModel::BodyRole) != "body" ||
        roles.value(MessageListModel::StatusRole) != "status" ||
        roles.value(MessageListModel::GroupedWithPreviousRole) !=
            "groupedWithPrevious" ||
        model.data(first, MessageListModel::AuthorNameRole).toString() != "Mina" ||
        model.data(first, MessageListModel::TimestampRole).toString() != "09:00" ||
        model.data(first, MessageListModel::BodyRole).toString() != "First" ||
        model.data(first, MessageListModel::StatusRole).toInt() !=
            MessageListModel::Sent ||
        model.data(first, MessageListModel::GroupedWithPreviousRole).toBool() ||
        !model.data(second, MessageListModel::GroupedWithPreviousRole).toBool() ||
        model.data(third, MessageListModel::GroupedWithPreviousRole).toBool() ||
        model.data({}, MessageListModel::BodyRole).isValid()) {
        return EXIT_FAILURE;
    }

    MessageListModel::Message updated_second{
        "message-2",
        "user-mina",
        "Mina",
        "09:01",
        "Second updated",
        MessageListModel::Failed,
    };
    if (!model.updateMessage(updated_second) || data_changes.size() != 1 ||
        data_changes.constFirst().first_row != 1 ||
        data_changes.constFirst().last_row != 1 ||
        data_changes.constFirst().roles !=
            QList<int>{MessageListModel::BodyRole, MessageListModel::StatusRole} ||
        model.data(second, MessageListModel::BodyRole).toString() != "Second updated" ||
        model.data(second, MessageListModel::StatusRole).toInt() !=
            MessageListModel::Failed ||
        !model.updateMessage(updated_second) || data_changes.size() != 1) {
        return EXIT_FAILURE;
    }

    updated_second.author_id = "user-alex";
    updated_second.author_name = "Alex";
    if (!model.updateMessage(updated_second) || data_changes.size() != 3 ||
        data_changes.at(1).first_row != 1 || data_changes.at(1).last_row != 1 ||
        data_changes.at(1).roles !=
            QList<int>{MessageListModel::AuthorIdRole,
                       MessageListModel::AuthorNameRole,
                       MessageListModel::GroupedWithPreviousRole} ||
        data_changes.at(2).first_row != 2 || data_changes.at(2).last_row != 2 ||
        data_changes.at(2).roles !=
            QList<int>{MessageListModel::GroupedWithPreviousRole} ||
        model.data(second, MessageListModel::GroupedWithPreviousRole).toBool() ||
        !model.data(third, MessageListModel::GroupedWithPreviousRole).toBool()) {
        return EXIT_FAILURE;
    }

    const MessageListModel::Message missing{
        "message-missing",
        "user-mina",
        "Mina",
        "09:06",
        "Missing",
        MessageListModel::Sent,
    };
    auto invalid = updated_second;
    invalid.body.clear();
    if (model.updateMessage(missing) || model.updateMessage(invalid) ||
        model.rowCount() != 4 || model_resets != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
