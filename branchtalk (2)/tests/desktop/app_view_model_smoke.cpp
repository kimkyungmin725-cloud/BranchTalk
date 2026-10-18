#include "app_view_model.hpp"

#include <QObject>

#include <cstdlib>

int main()
{
    using branchtalk::desktop::AppViewModel;

    AppViewModel view_model;
    int change_count = 0;
    QObject::connect(&view_model,
                     &AppViewModel::currentScreenChanged,
                     [&change_count]
                     { ++change_count; });

    if (view_model.currentScreen() != AppViewModel::LoginScreen)
    {
        return EXIT_FAILURE;
    }

    view_model.showLogin();
    if (change_count != 0)
    {
        return EXIT_FAILURE;
    }

    view_model.showMain();
    if (view_model.currentScreen() != AppViewModel::MainScreen || change_count != 1)
    {
        return EXIT_FAILURE;
    }

    view_model.showMain();
    if (change_count != 1)
    {
        return EXIT_FAILURE;
    }

    view_model.showSettings();
    if (view_model.currentScreen() != AppViewModel::SettingsScreen || change_count != 2)
    {
        return EXIT_FAILURE;
    }

    view_model.showLogin();
    return view_model.currentScreen() == AppViewModel::LoginScreen && change_count == 3
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
}
