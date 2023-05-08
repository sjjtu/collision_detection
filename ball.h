#pragma once
class Ball {
    public:
        float radius;
        float mass;
        float position_x;
        float position_y;
        float velocity_x;
        float velocity_y;
        float id;
        Ball();
        Ball(float r , float m, float pos_x, float pos_y, float vel_x, float vel_y, float id);

        void update(float dt);  
            

        bool collision_detected();

};
