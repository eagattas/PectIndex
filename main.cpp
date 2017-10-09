#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include "pectusviewer.h"
#include "pectusprocessor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

    QQmlApplicationEngine engine;
    PectusViewer myViewer;
    PectusProcessor myProcessor;
    QQmlContext* context = engine.rootContext();
    context->setContextProperty("myViewer", &myViewer);
    context->setContextProperty("myProcessor", &myProcessor);
    qmlRegisterUncreatableType<PectusViewer>("PectusViewer", 1, 1, "PectusViewer", "Do not instantiate");
    qmlRegisterUncreatableType<PectusProcessor>("PectusProcessor", 1, 1, "PectusProcessor", "Do not instantiate");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
