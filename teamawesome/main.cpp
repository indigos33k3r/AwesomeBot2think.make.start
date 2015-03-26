
#include <string>
#include <iostream>

#include <QtCore>
#include <QApplication>

#include <rec/robotino/api2/all.h>

#include "com.h"
#include "cam3d.h"
#include "gamepad.h"
#include "robotinocontrol.h"
#include "mainwindow.h"
#include "renderwindow.h"
#include "dxlcon.h"
#include "oculussensor.h"

// Global pointers
Com* com;


int main (int argc, char** argv) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    qRegisterMetaType <cv::Mat> ("cv::Mat");

    QCoreApplication a(argc, argv);


    try {
        // Set up Robotino connection
        com = new Com("Awesome", "172.26.1.1");
        if(! com->isConnected() ) {
            std::cerr << "could not connect to robot" << std::endl;
            rec::robotino::api2::shutdown();
            exit(1);
        }
        std::cout << "connected." << std::endl;

        // Dynamixel Ansteuerung
        DxlCon dxlCon;
        dxlCon.setDxlPos(DxlCon::YAW, 0);
        dxlCon.setDxlPos(DxlCon::ROLL, 0);
        dxlCon.setDxlPos(DxlCon::PITCH, 0);

        // Cam3D erstellen
        QThread threadCam;
        threadCam.setObjectName("Camera Thread");
        Cam3D cam;
        cam.moveToThread(&threadCam);
        threadCam.start();

        // renderWindow für Oculus Display
        RenderWindow render;
        QObject::connect(&cam, SIGNAL(signalImage(cv::Mat)), &render, SLOT(slotFrame(cv::Mat)));

        // Gamepad erstellen
        QThread* threadOfJoy = new QThread();
        Gamepad joystick;
        joystick.moveToThread(threadOfJoy);
        threadOfJoy->setObjectName("Thread of Joy");

        threadOfJoy->start();

        // Driving Control of the Robotino
        RobotinoControl robotinoControl;
        QObject::connect(&joystick, SIGNAL(setCarLike(double,double,double)), &robotinoControl, SLOT(setCarLike(double,double,double)));
        QObject::connect(&joystick, SIGNAL(setView(double,double,double)), &dxlCon, SLOT(setDxlPos(double,double,double)));

        // Oculus Sensoren
        OculusSensor oculus;
        QObject::connect(&oculus, SIGNAL(signalSensorData(double,double,double)), &dxlCon, SLOT(setDxlPos(double,double,double)));

        // Joystick loop starten
        QMetaObject::invokeMethod(&joystick, "run");

        // das ist die hauptschleife, später vlt in eigenes qobject auf eigenem thread?
        while(com->isConnected()) {

            QCoreApplication::processEvents();
            com->processEvents();

//            rec::robotino::api2::msleep( 20 );
//            QThread::msleep(20);

        }


    } catch ( ... ) {
        std::cerr << "an error occurred." << std::endl;
        exit(1);
        rec::robotino::api2::shutdown();
    }


}
