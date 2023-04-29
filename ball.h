#pragma once
class Ball {
    public:
        float radius;
        float mass;
        float position_x;
        float position_y;
        float velocity_x;
        float velocity_y;
        Ball();
        Ball(float r , float m, float pos_x, float pos_y, float vel_x, float vel_y);

        void update(float dt);  
            

        bool collision_detected();

};
