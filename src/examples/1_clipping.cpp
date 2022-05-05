#include "../SlimRaster/scene/selection.h"
#include "../SlimRaster/draw/hud.h"
#include "../SlimRaster/draw/selection.h"
#include "../SlimRaster/renderer/rasterizer.h"
#include "../SlimRaster/renderer/pixel_shaders.h"
#include "../SlimRaster/renderer/mesh_shaders.h"
#include "../SlimRaster/app.h"
// Or using the single-header file:
// #include "../SlimRaster.h"

struct ClippingExample : SlimRaster {
    // Viewport:
    Camera camera{
            {0, 10, -15},
            {-25*DEG_TO_RAD,0, 0}
    }, *cameras{&camera};
    Viewport viewport{window::canvas, &camera};

    HUDLine Fps{      (char*)"Fps      : "};
    HUDLine Wireframe{(char*)"Wireframe: ",
                      (char*)"Off",
                      (char*)"On",
                      &viewport.show_wireframe, Grey};
    HUDLine Antialias{(char*)"Antialias: ",
                      (char*)"Off",
                      (char*)"On",
                      &window::canvas.antialias, Grey},
                      *hud_lines{&Fps};
    HUDSettings hud_settings{3,1.2f};
    HUD hud{hud_settings, hud_lines};

    // Scene:
    char string[100] = {};
    String mesh_file{String::getFilePath((char*)"suzanne.mesh",string,(char*)__FILE__)};
    Mesh mesh;

    Geometry floor{{{-6, -3, 0}, {}, {16, 1, 16}}, GeometryType_Box};
    Geometry monkey1{{{}, {0, 0.5f, 0} },GeometryType_Mesh, White, 0, 1};
    Geometry monkey2{{{2, 2, 3} },       GeometryType_Mesh, White, 0, 2};
    Geometry monkey3{{{-2, 2, -3} },     GeometryType_Mesh, White, 0, 3},
            *geometries{&floor};

    Material floor_material{  shadePixelCheckerboard, shadeMesh};
    Material monkey_material1{shadePixelPosition,     shadeMesh};
    Material monkey_material2{shadePixelUV,           shadeMesh};
    Material monkey_material3{shadePixelNormal,       shadeMesh},
            *materials{&floor_material};

    SceneCounts counts{1, 4, 0, 1, 0, 1, 4};
    Scene scene{counts,nullptr, cameras, geometries, nullptr, nullptr,nullptr,
                materials, nullptr, &mesh, &mesh_file};

    Selection selection;

    Rasterizer raterizer{scene};

    void OnRender() override {
        Fps.value = (i32)render_timer.average_frames_per_second;
        raterizer.rasterize(viewport);

        if (controls::is_pressed::alt) draw(selection, viewport, scene);
        if (hud.enabled) draw(hud, viewport);
    }

    void OnKeyChanged(u8 key, bool is_pressed) override {
        if (!is_pressed) {
            if (key == controls::key_map::tab) hud.enabled = !hud.enabled;
            if (controls::is_pressed::ctrl) {
                if (key == 'W') viewport.show_wireframe = !viewport.show_wireframe;
                if (key == 'A') viewport.canvas.antialias = !viewport.canvas.antialias;
            }
        }

        NavigationMove &move = viewport.navigation.move;
        NavigationTurn &turn = viewport.navigation.turn;
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
        if (!mouse::is_captured) selection.manipulate(viewport, scene);
        if (!controls::is_pressed::alt) viewport.updateNavigation(delta_time);
    }

    void OnWindowResize(u16 width, u16 height) override {
        viewport.frustum.near_clipping_plane_distance = 1;
        viewport.updateDimensions(width, height);
        viewport.updateProjection();
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

SlimRaster* createEngine() {
    return (SlimRaster*)new ClippingExample();
}