#pragma once

#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

namespace branchtalk::desktop {

class MessageComposerState : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("MessageComposerState is provided by the application")

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)
    Q_PROPERTY(bool canSend READ canSend NOTIFY canSendChanged)

public:
    explicit MessageComposerState(QObject* parent = nullptr);

    [[nodiscard]] const QString& text() const noexcept;
    [[nodiscard]] int characterCount() const noexcept;
    [[nodiscard]] bool canSend() const noexcept;

    void setText(QString text);
    Q_INVOKABLE bool submit(bool input_method_composing);

signals:
    void textChanged();
    void characterCountChanged();
    void canSendChanged();
    void messageSubmitted(const QString& text);

private:
    QString text_;
};

}  // namespace branchtalk::desktop
