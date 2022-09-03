#include "raylib.h"
#include "raymath.h"
#include "global.h"
#include <chrono> // std::chrono::microseconds
#include <thread> // std::this_thread::sleep_for
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <raylib.h>


#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else 
#define GLSL_VERSION            100
#endif

int score;
bool exitWindow;
bool xd;

void waiter()
{
    using namespace std::chrono;
    std::this_thread::sleep_for(10000ms);
    xd = false;
    std::this_thread::sleep_for(3000ms);
    exit(0);
}

int main(void)
{

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    
    exitWindow = false;
    xd = true;

    std::thread thread_obj(waiter);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - basic lighting");
    ToggleFullscreen();

    Camera camera = { 0 };
    camera.position = Vector3{ 2.0f, 4.0f, 6.0f };    
    camera.target = Vector3{ 0.0f, 0.5f, 0.0f };     
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };        
    camera.fovy = 30.0f;                        
    camera.projection = CAMERA_PERSPECTIVE;             


    Model model = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 3, 3));
    Model cube = LoadModelFromMesh(GenMeshCube(2.0f, 4.0f, 2.0f));


    Shader shader = LoadShader(TextFormat("C:/Users/simon/source/repos/benchmark/benchmark/base_lighting.vs", GLSL_VERSION),
        TextFormat("C:/Users/simon/source/repos/benchmark/benchmark/lighting.fs", GLSL_VERSION));
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    
    int ambientLoc = GetShaderLocation(shader, "ambient");
    float nevim[5] = { 0.1f, 0.1f, 0.1f, 1.0f, 0.1f};

    SetShaderValue(shader, ambientLoc, nevim, SHADER_UNIFORM_VEC4);

    model.materials[0].shader = shader;
    cube.materials[0].shader = shader;

    Light lights[5] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, Vector3 { -2, 1, -2 }, Vector3Zero(), YELLOW, shader);
    lights[1] = CreateLight(LIGHT_POINT, Vector3 { 2, 1, 2 }, Vector3Zero(), RED, shader);
    lights[2] = CreateLight(LIGHT_POINT, Vector3 { -2, 1, 2 }, Vector3Zero(), GREEN, shader);
    lights[3] = CreateLight(LIGHT_POINT, Vector3 { 2, 1, -2 }, Vector3Zero(), BLUE, shader);

    SetCameraMode(camera, CAMERA_ORBITAL); 

    while (exitWindow == false)          
    {
        if (xd) {

            UpdateCamera(&camera);
            float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
            SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

            for (int i = 0; i < 5; i++) UpdateLightValues(shader, lights[i]);

            BeginDrawing();
            score = score + GetFPS();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

            DrawModel(model, Vector3Zero(), 1.0f, WHITE);
            DrawModel(cube, Vector3Zero(), 1.0f, WHITE);

            for (int i = 0; i < 5; i++)
            {
                if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
                else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
            }

            DrawGrid(10, 1.0f);
            EndMode3D();
            DrawText("10 SEC GPU TEST", 10, 10, 20, DARKGRAY);
            EndDrawing();
        }
        else {
            BeginDrawing();
            SetTargetFPS(60);
            ClearBackground(RAYWHITE);
            DrawText("Your score:", 15, 15, 80, DARKGRAY);

            std::string s = std::to_string(score / 69420);
            std::string s2 = s.substr(0, s.length());
            const char* s3 = s2.c_str();
            
            DrawText(s3, 15, 150, 80, RED);
            EndDrawing();
        }
    }

    UnloadModel(model); 
    UnloadModel(cube);
    UnloadShader(shader);

    CloseWindow();         

    return 0;
}