#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "selectionmodel.h"
#include "filemanager.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    qmlRegisterType< SelectionModel >("CppBackend", 1, 0, "SelectionModel");
    qmlRegisterType< FileManager >("CppBackend", 1, 0, "FileManager");

    QGuiApplication app(argc, argv);

    auto model = std::make_shared<SelectionModel>();

    auto manager = std::make_shared<FileManager>();

    manager->setModel(model);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("backendModel", model.get());
    engine.rootContext()->setContextProperty("backendManager", manager.get());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
    {
        return -1;
    }

    return app.exec();
}
