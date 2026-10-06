#include "app_view_model.hpp"

namespace branchtalk::desktop
{

    AppViewModel::AppViewModel(QObject *parent) : QObject{parent} {}

    AppViewModel::Screen AppViewModel::currentScreen() const noexcept
    {
        return current_screen_;
    }

    void AppViewModel::showLogin()
    {
        setCurrentScreen(LoginScreen);
    }

    void AppViewModel::showMain()
    {
        setCurrentScreen(MainScreen);
    }

    void AppViewModel::showSettings()
    {
        setCurrentScreen(SettingsScreen);
    }

    void AppViewModel::setCurrentScreen(Screen screen)
    {
        if (current_screen_ == screen)
        {
            return;
        }

        current_screen_ = screen;
        emit currentScreenChanged();
    }

} // namespace branchtalk::desktop
