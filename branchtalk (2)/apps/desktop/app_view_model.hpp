#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

namespace branchtalk::desktop
{

    class AppViewModel : public QObject
    {
        Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("AppViewModel is provided by the application")

    public:
        enum Screen
        {
            LoginScreen,
            MainScreen,
            SettingsScreen,
        };
        Q_ENUM(Screen)

        Q_PROPERTY(Screen currentScreen READ currentScreen NOTIFY currentScreenChanged)

        explicit AppViewModel(QObject *parent = nullptr);

        [[nodiscard]] Screen currentScreen() const noexcept;

        Q_INVOKABLE void showLogin();
        Q_INVOKABLE void showMain();
        Q_INVOKABLE void showSettings();

    signals:
        void currentScreenChanged();

    private:
        void setCurrentScreen(Screen screen);

        Screen current_screen_{LoginScreen};
    };

} // namespace branchtalk::desktop
