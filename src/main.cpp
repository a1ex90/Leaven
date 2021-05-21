/******************************************************
 *
 *   #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *  %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *  %    %## #    CC        V    V  MM M  M MM RR    RR
 *   ,%      %    CC        VV  VV  MM  MM  MM RRRRRR
 *   (%      %,   CC    CC   VVVV   MM      MM RR   RR
 *     #%    %*    CCCCCC     VV    MM      MM RR    RR
 *    .%    %/
 *       (%.      Computer Vision & Mixed Reality Group
 *
 *****************************************************/
/** @copyright:   Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author:   Alex Sommer
 *    @version:   1.0
 *       @date:   19.11.2020
 *****************************************************/

#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "gui/backend.h"
#include "gui/openGLWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QFont font = qApp->font();
    font.setPixelSize(12);
    qApp->setFont(font);

    qmlRegisterType<Backend>("sampler.qt.backend", 1, 0, "SamplerBackend");
    qmlRegisterType<FBO>("opengl.qt.viewer", 1, 0, "RenderUnit");
    qRegisterMetaType<Settings*>("Settings*");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    auto* fb = qobject_cast<FBO*>(engine.rootObjects().first()->findChild<QObject*>("renderunit_id"));
    auto* bcknd = qobject_cast<Backend*>(engine.rootObjects().first()->findChild<QObject*>("bcknd_id"));
    QObject::connect(bcknd, &Backend::addGeometry, fb, &FBO::receiveGeometry);
    QObject::connect(bcknd, &Backend::callUpate, fb, &FBO::update);
    QObject::connect(bcknd, &Backend::setViewCenter, fb, &FBO::setCenter);

    return app.exec();
}