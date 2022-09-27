#pragma once

#include "../scene/camera.h"

struct Navigation {
    struct {
        struct {
            f32 turn{   NAVIGATION_SPEED_DEFAULT__TURN  };
            f32 zoom{   NAVIGATION_SPEED_DEFAULT__ZOOM  };
            f32 dolly{  NAVIGATION_SPEED_DEFAULT__DOLLY };
            f32 pan{    NAVIGATION_SPEED_DEFAULT__PAN   };
            f32 orbit{  NAVIGATION_SPEED_DEFAULT__ORBIT };
            f32 orient{ NAVIGATION_SPEED_DEFAULT__ORIENT};
        } speed;
        f32 max_velocity{NAVIGATION_DEFAULT__MAX_VELOCITY};
        f32 acceleration{NAVIGATION_DEFAULT__ACCELERATION};
    } settings;

    Move move{};
    Turn turn{};

    bool zoomed{false};
    bool moved{false};
    bool turned{false};

    void pan(Camera &camera) {
        camera.pan(settings.speed.pan * -(f32)mouse::pos_raw_diff_x,
                   settings.speed.pan * +(f32)mouse::pos_raw_diff_y);
        moved = true;
        mouse::moved = false;
    }

    void zoom(Camera &camera) {
        camera.zoom(settings.speed.zoom * mouse::wheel_scroll_amount);
        zoomed = true;
    }

    void dolly(Camera &camera) {
        camera.dolly(settings.speed.dolly * mouse::wheel_scroll_amount);
        moved = true;
    }

    void orient(Camera &camera) {
        camera.rotate(settings.speed.orient * -(f32)mouse::pos_raw_diff_y,
                      settings.speed.orient * -(f32)mouse::pos_raw_diff_x);
        mouse::moved = false;
        turned = true;
    }

    void orbit(Camera &camera) {
        camera.orbit(settings.speed.orbit * -(f32)mouse::pos_raw_diff_x,
                     settings.speed.orbit * -(f32)mouse::pos_raw_diff_y);
        turned = true;
        moved = true;
        mouse::moved = false;
    }

    void navigate(Camera &camera, f32 delta_time) {
        vec3 target_velocity;
        if (move.right)    target_velocity.x += settings.max_velocity;
        if (move.left)     target_velocity.x -= settings.max_velocity;
        if (move.up)       target_velocity.y += settings.max_velocity;
        if (move.down)     target_velocity.y -= settings.max_velocity;
        if (move.forward)  target_velocity.z += settings.max_velocity;
        if (move.backward) target_velocity.z -= settings.max_velocity;
        if (turn.left) {
            camera.rotateAroundY(delta_time * settings.speed.turn);
            turned = true;
        }
        if (turn.right) {
            camera.rotateAroundY(delta_time * -settings.speed.turn);
            turned = true;
        }

        // Update the current speed_x and position_x:
        vec3 &velocity = camera.current_velocity;
        velocity = velocity.approachTo(target_velocity,
                                       settings.acceleration * delta_time);
        vec3 movement = velocity * delta_time;
        moved = movement.nonZero();
        if (moved) camera.position += camera.rotation * movement;
    }

    void update(Camera &camera, f32 delta_time) {
        if (mouse::is_captured) {
            navigate(camera, delta_time);
            if (mouse::moved) orient(camera);
            if (mouse::wheel_scrolled) zoom(camera);
        } else {
            if (mouse::wheel_scrolled) dolly(camera);
            if (mouse::moved) {
                if (mouse::middle_button.is_pressed) pan(camera);
                if (mouse::right_button.is_pressed) orbit(camera);
            }
        }
    }
};