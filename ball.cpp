 #include "ball.h"
 
 Ball::Ball(){};

 Ball::Ball(float r , float m, float pos_x, float pos_y, float vel_x, float vel_y, int i){
            radius = r;
            mass = m;
            position_x = pos_x;
            position_y = pos_y;
            velocity_y = vel_y;
            velocity_x = vel_x;
            id = i;
        }
void Ball::update(float dt){
    this->position_x = position_x + dt*velocity_x;
    this->position_y = position_y + dt*velocity_y;
};





