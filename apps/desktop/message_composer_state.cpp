#include "message_composer_state.hpp"

#include <utility>

namespace branchtalk::desktop {

MessageComposerState::MessageComposerState(QObject* parent) : QObject{parent} {}

const QString& MessageComposerState::text() const noexcept {
    return text_;
}

int MessageComposerState::characterCount() const noexcept {
    return static_cast<int>(text_.size());
}

bool MessageComposerState::canSend() const noexcept {
    return !text_.trimmed().isEmpty();
}

void MessageComposerState::setText(QString text) {
    if (text_ == text) {
        return;
    }

    const bool previously_can_send = canSend();
    text_ = std::move(text);
    emit textChanged();
    emit characterCountChanged();
    if (previously_can_send != canSend()) {
        emit canSendChanged();
    }
}

bool MessageComposerState::submit(bool input_method_composing) {
    if (input_method_composing || !canSend()) {
        return false;
    }

    const QString submitted_text = text_;
    setText({});
    emit messageSubmitted(submitted_text);
    return true;
}

}  // namespace branchtalk::desktop
