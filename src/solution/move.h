#pragma once

#define WHEEL_TO_SENSOR 1.8f
#define WHEEL_SPACING 6

void move_straight(float percent);
void stop();

void move_turn(float percent, float turn_speed);
void move_radius(float percent, float turn_rad);

void move_follow(float percent);

void move_follow_fancy(float percent);