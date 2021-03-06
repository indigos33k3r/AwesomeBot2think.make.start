#ifndef LASER_H
#define LASER_H

#include <QObject>

class SerialPort;

class Laser : public QObject
{
    Q_OBJECT
public:
    explicit Laser(QString path = "/dev/ttyACM0", QObject *parent = 0);
    ~Laser();

    // commands:
    typedef enum  {
      LASERON = 0,
      LASEROFF,
      MEAS_NORMAL,
      MEAS_CONT_START,
      MEAS_FAST,
      MEAS_CONT_STOP,

      NUM_CMDS
    }COMMANDS;

    void laserOn();
    void laserOff();
    double measure();

    int SendCommand(COMMANDS cmd, char *rx, int rxbytes, int timeout);
    int MeasureDistance(double *Value, COMMANDS cmd);


signals:

public slots:

private:
    SerialPort* port;

#define  CRC8_POLYNOMIAL   0xA6
#define CRC8_INITIAL_VALUE 0xAA

    static const char CMD[NUM_CMDS][10];
    static const int LEN[NUM_CMDS];
    static const int RET_NOERR         = 1;
    static const int RET_ERR_TIMEOUT   =-1;
    static const int RET_ERR_STATUS    =-2;

    unsigned char CalcCrc8FromArray(const unsigned char *pData, unsigned short NumBytes, unsigned char InitialValue);
    unsigned char CalcCrc8(unsigned char Data, unsigned char InitialValue);
};

#endif // LASER_H
