
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

// Global pointers
Com* com;


int main (int argc, char** argv) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    qRegisterMetaType <cv::Mat> ("cv::Mat");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    try {
        // Set up Robotino connection
        com = new Com("Awesome", "172.26.1.1");
        if(! com->isConnected() ) {
            std::cerr << "could not connect to robot" << std::endl;
            rec::robotino::api2::shutdown();
            exit(1);
        }
        std::cout << "connected." << std::endl;

        // Cam3D erstellen
        Cam3D cam;

        // Gamepad erstellen
        QThread* threadOfJoy = new QThread();
        Gamepad joystick;

        joystick.moveToThread(threadOfJoy);
        threadOfJoy->setObjectName("Thread of Joy");
        threadOfJoy->start();

        // Driving Control of the Robotino
        RobotinoControl robotinoControl;

        QObject::connect(&joystick, SIGNAL(setCarLike(double,double,double)), &robotinoControl, SLOT(setCarLike(double,double,double)));
        joystick.publish();

        // das ist die hauptschleife, später vlt in eigenes qobject auf eigenem thread?
        while(com->isConnected()) {
            com->processEvents();

            rec::robotino::api2::msleep( 100 );
        }


    } catch ( ... ) {
        std::cerr << "an error occurred." << std::endl;
        exit(1);
        rec::robotino::api2::shutdown();
    }
    return a.exec();


}
