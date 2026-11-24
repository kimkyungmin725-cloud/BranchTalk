#include "message_composer_state.hpp"

#include <QObject>
#include <QString>

#include <cstdlib>

int main() {
    using branchtalk::desktop::MessageComposerState;

    MessageComposerState state;
    int text_change_count = 0;
    int character_count_change_count = 0;
    int can_send_change_count = 0;
    int submission_count = 0;
    QString submitted_text;

    QObject::connect(&state,
                     &MessageComposerState::textChanged,
                     [&text_change_count] { ++text_change_count; });
    QObject::connect(&state,
                     &MessageComposerState::characterCountChanged,
                     [&character_count_change_count] {
                         ++character_count_change_count;
                     });
    QObject::connect(&state,
                     &MessageComposerState::canSendChanged,
                     [&can_send_change_count] { ++can_send_change_count; });
    QObject::connect(&state,
                     &MessageComposerState::messageSubmitted,
                     [&submission_count, &submitted_text](const QString& text) {
                         ++submission_count;
                         submitted_text = text;
                     });

    if (!state.text().isEmpty() || state.characterCount() != 0 || state.canSend()) {
        return EXIT_FAILURE;
    }

    state.setText(QStringLiteral(" \n"));
    if (state.text() != QStringLiteral(" \n") || state.characterCount() != 2 ||
        state.canSend() || text_change_count != 1 ||
        character_count_change_count != 1 || can_send_change_count != 0 ||
        state.submit(false)) {
        return EXIT_FAILURE;
    }

    const auto greeting = QString::fromUtf8(
        "\xEC\x95\x88\xEB\x85\x95\xED\x95\x98\xEC\x84\xB8\xEC\x9A\x94");
    state.setText(greeting);
    if (state.characterCount() != 5 || !state.canSend() || text_change_count != 2 ||
        character_count_change_count != 2 || can_send_change_count != 1) {
        return EXIT_FAILURE;
    }

    state.setText(greeting);
    if (text_change_count != 2 || character_count_change_count != 2 ||
        can_send_change_count != 1) {
        return EXIT_FAILURE;
    }

    if (state.submit(true) || submission_count != 0 || state.text() != greeting ||
        !state.canSend()) {
        return EXIT_FAILURE;
    }

    if (!state.submit(false) || submission_count != 1 || submitted_text != greeting ||
        !state.text().isEmpty() || state.characterCount() != 0 || state.canSend() ||
        text_change_count != 3 || character_count_change_count != 3 ||
        can_send_change_count != 2) {
        return EXIT_FAILURE;
    }

    const auto multiline = QString::fromUtf8(
        "\xEC\xB2\xAB \xEC\xA4\x84\n\xEB\x91\x98\xEC\xA7\xB8 \xEC\xA4\x84");
    state.setText(multiline);
    if (state.characterCount() != 8 || !state.canSend() || !state.submit(false) ||
        submission_count != 2 || submitted_text != multiline) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
