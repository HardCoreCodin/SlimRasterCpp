#include "../slim/scene/selection.h"
#include "../slim/draw/hud.h"
#include "../slim/draw/selection.h"
#include "../slim/renderer/rasterizer.h"
#include "../slim/renderer/pixel_shaders.h"
#include "../slim/renderer/mesh_shaders.h"
#include "../slim/app.h"
// Or using the single-header file:
//#include "../slim.h"

struct NormalMapsExample : SlimApp {
    // Viewport:
    Camera camera{
            {0, 15, -15},
            {-25*DEG_TO_RAD,0, 0}
    }, *cameras{&camera};
    Canvas canvas;
    Viewport viewport{canvas, &camera};

    bool draw_wireframe = false;

    HUDLine Fps{      (char*)"Fps      : "};
    HUDLine Wireframe{(char*)"Wireframe: ",
                      (char*)"Off",
                      (char*)"On",
                      &draw_wireframe, true, Grey};
    HUDLine Antialias{(char*)"Antialias: ", Grey};
    HUDLine NormalMagnitude{(char*)"Normal Magnitude: "}, *hud_lines{&Fps};

    HUDSettings hud_settings{4,1.2f};
    HUD hud{hud_settings, hud_lines};

    // Scene:
    Geometry floor{{{-6, -3, 0}, {}, {16, 1, 16}                       }, GeometryType_Box};
    Geometry dog{  {{2, 2, 9}, {0, 120*DEG_TO_RAD, 0}, vec3{0.5f} }, GeometryType_Mesh,
                   White, 0, 1},
             *geometries{&floor};

    char strings[5][100] = {};
    String mesh_file{String::getFilePath((char*)"dog.mesh",       strings[0],(char*)__FILE__)};
    String texture_files[4]{
            String::getFilePath((char*)"floor_albedo.texture",strings[1],(char*)__FILE__),
            String::getFilePath((char*)"floor_normal.texture",strings[2],(char*)__FILE__),
            String::getFilePath((char*)"dog_albedo.texture",  strings[3],(char*)__FILE__),
            String::getFilePath((char*)"dog_normal.texture",  strings[4],(char*)__FILE__)
    };
    Texture textures[4];

    Material floor_material{shadePixelLighting, shadeMesh,2,
        vec3{0.7f}, vec3{1.0f}, 1.0f, 1.0f, 0.4f};
    Material dog_material{  shadePixelLighting, shadeMesh, 2,
        vec3{0.4f}, vec3{1.0f}, 1.0f, 1.0f, 3.0f},
        *materials{&floor_material};

    Light light1{{  -3, 5,  8}, {0.8f, 0.3f, 0.2f}, 20};
    Light light2{{   3, 4,  8}, {0.2f, 0.3f, 0.8f}, 20};
    Light light3{{2.5f, 3, -1}, {0.2f, 0.9f, 0.3f}, 16},
        *lights{&light1};

    Mesh mesh;

    SceneCounts counts{1, 2, 0, 1, 0, 1, 4, 4, 3};
    Scene scene{counts,nullptr, cameras, geometries,nullptr, nullptr,nullptr,
                materials, lights, &mesh, &mesh_file, textures, texture_files};
    Selection selection;
    Rasterizer raterizer{scene};

    NormalMapsExample() {
        viewport.frustum.near_clipping_plane_distance = 1;
        viewport.updateProjection();

        scene.ambient_light.color = {0.008f, 0.008f, 0.014f};

        floor_material.texture_ids[0] = 0;
        floor_material.texture_ids[1] = 1;

        dog_material.texture_ids[0] = 2;
        dog_material.texture_ids[1] = 3;
    }

    void OnRender() override {
        canvas.clear();
        Fps.value = (i32)render_timer.average_frames_per_second;
        raterizer.rasterize(viewport);

        if (controls::is_pressed::alt) drawSelection(selection, viewport, scene);
        if (hud.enabled) drawHUD(hud, canvas);
        canvas.drawToWindow();
    }

    void OnKeyChanged(u8 key, bool is_pressed) override {
        if (!is_pressed) {
            if (key == controls::key_map::tab) hud.enabled = !hud.enabled;
            if (controls::is_pressed::ctrl) {
                if (key == 'W') draw_wireframe = !draw_wireframe;
                if (key == 'A') viewport.canvas.antialias = viewport.canvas.antialias == NoAA ? SSAA : NoAA;
            }
        }

        Move &move = viewport.navigation.move;
        Turn &turn = viewport.navigation.turn;
        if (key == 'Q') turn.left     = is_pressed;
        if (key == 'E') turn.right    = is_pressed;
        if (key == 'R') move.up       = is_pressed;
        if (key == 'F') move.down     = is_pressed;
        if (key == 'W') move.forward  = is_pressed;
        if (key == 'S') move.backward = is_pressed;
        if (key == 'A') move.left     = is_pressed;
        if (key == 'D') move.right    = is_pressed;
    }

    void OnUpdate(f32 delta_time) override {
        static float elapsed = 0;
        elapsed += delta_time;

        light1.position_or_direction = {
                floor.transform.position.x - 3.0f + sinf(elapsed) * 0.6f,
                2 + sinf(elapsed * 2.0f),
                floor.transform.position.z + 3.0f + cosf(elapsed) * 0.6f
        };
        light3.position_or_direction = {
                dog.transform.position.x + 3.0f + sinf(elapsed * 0.5f) * 0.6f,
                2 + cosf(elapsed * 2.0f),
                dog.transform.position.z + 3.0f + cosf(elapsed * 0.5f) * 0.6f
        };

        if (!mouse::is_captured) selection.manipulate(viewport, scene);
        Material &material = scene.materials[scene.geometries[selection.geo_id].material_id];

        bool wheel_scroll_handled = false;
        if (controls::is_pressed::ctrl) {
            if (mouse::wheel_scrolled) {
                mouse::wheel_scrolled = false;
                wheel_scroll_handled = true;
                material.normal_magnitude += mouse::wheel_scroll_amount * 0.001f;
                material.normal_magnitude = clampedValue(material.normal_magnitude, 0, 4);
                NormalMagnitude.value = material.normal_magnitude;
            }
        } else if (!controls::is_pressed::alt) viewport.updateNavigation(delta_time);

        if (selection.changed) {
            selection.changed = false;
            if (!wheel_scroll_handled)
                NormalMagnitude.value = material.normal_magnitude;
        }
    }

    void OnWindowResize(u16 width, u16 height) override {
        viewport.updateDimensions(width, height);
        canvas.dimensions.update(width, height);
        dog_material.roughness = floor_material.roughness = (f32)(width >> 1);
    }

    void OnMouseButtonDown(mouse::Button &mouse_button) override {
        mouse::pos_raw_diff_x = mouse::pos_raw_diff_y = 0;
    }

    void OnMouseButtonDoubleClicked(mouse::Button &mouse_button) override {
        if (&mouse_button == &mouse::left_button) {
            mouse::is_captured = !mouse::is_captured;
            os::setCursorVisibility(!mouse::is_captured);
            os::setWindowCapture(    mouse::is_captured);
            OnMouseButtonDown(mouse_button);
        }
    }
};

SlimApp* createApp() {
    return new NormalMapsExample();
}