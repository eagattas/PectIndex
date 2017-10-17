#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <QQmlEngine>

#include "pectusviewer.h"
#include "pectusprocessor.h"

#include <QDebug>

int main(int argc, char *argv[])
{

    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    PectusProcessor myProcessor;
    PectusViewer myViewer;

    QQmlContext* context = engine.rootContext();

    context->setContextProperty("myViewer", &myViewer);
    qmlRegisterUncreatableType<PectusViewer>("PectusViewer", 1, 1, "PectusViewer", "Do not instantiate");

    context->setContextProperty("myProcessor", &myProcessor);
    qmlRegisterUncreatableType<PectusProcessor>("PectusProcessor", 1, 1, "PectusProcessor", "Do not instantiate");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    myProcessor.setRootQmlObject(engine.rootObjects().first());

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
