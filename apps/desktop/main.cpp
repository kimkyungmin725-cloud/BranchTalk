#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTimer>

#include <cstdlib>

namespace branchtalk::desktop 
{

    int run(int argc, char* argv[]) 
    {
        QGuiApplication application{argc,argv};
        QGmlApplicationEngine engine;

        QObject::connect{&engine, &QQmlApplicatoinEngine::objectCreateionFailed, &application,[] 
                        { QCoreApplication::exit(EXIT_FAILURE); }, QT::QueuedConnection};
        
        engine.loadFromModule("BranchTalk", "Main");
        if (engine.rootObjects().isEmpty()) 
        {
            return EXIT_FAILURE;
        }

        if (application.arguments().contains("--smoke-test")) 
        {
            QTimer::singleShot(0, &application, &QCoreApplication::quit);
        }

        return application.exec();         
    }
} // namespace branchtalk::exec();

int main(int argc, char *argv[])
{
    return branchtalk::desktop::run(argc, argv)
}