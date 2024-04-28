#ifndef MQ7SENSOR_H
#define MQ7SENSOR_H

class MQ7Sensor{
    public:
    int a;
    MQ7Sensor(){
        a = 4;
    }
    float getPPM(){
        return 30.0;
    }
};

#endif