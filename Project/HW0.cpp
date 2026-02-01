// GAMES101 HW0 

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#include <cmath>
#include <iostream>
#include <array>
#include <algorithm>

namespace math {

    struct Vec3 {
        float x{}, y{}, z{};
    };

    struct Mat3 {
        std::array<std::array<float, 3>, 3> m{};
    };

    constexpr Vec3 multiply(const Mat3& M, const Vec3& v) {
        return {
            M.m[0][0] * v.x + M.m[0][1] * v.y + M.m[0][2] * v.z,
            M.m[1][0] * v.x + M.m[1][1] * v.y + M.m[1][2] * v.z,
            M.m[2][0] * v.x + M.m[2][1] * v.y + M.m[2][2] * v.z
        };
    }

    Mat3 rotation(float angleDeg) {
        float rad = angleDeg * static_cast<float>(M_PI) / 180.0f;
        float c = std::cos(rad);
        float s = std::sin(rad);

        return { {
            {{ c, -s, 0 }},
            {{ s,  c, 0 }},
            {{ 0,  0, 1 }}
        } };
    }

    Mat3 translation(float tx, float ty) {
        return { {
            {{ 1, 0, tx }},
            {{ 0, 1, ty }},
            {{ 0, 0, 1  }}
        } };
    }

}


struct SDLSystem {
    SDLSystem() { SDL_Init(SDL_INIT_VIDEO); }
    ~SDLSystem() { SDL_Quit(); }
};

int main() {

    SDLSystem sdlGuard;

    SDL_Window* window = SDL_CreateWindow(
        "GAMES101 HW0 Animation (Modern C++)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL
    );

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5, 5, -5, 5, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    using namespace math;

    Vec3 P{ 2, 1, 1 };

    bool quit = false;
    SDL_Event e;

    int state = 0;
    float timer = 0.0f;

    while (!quit) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        // state
        timer += 0.016f;

        Vec3 P_rot = P;
        Vec3 P_final = P;

        //state
        if (state == 0) {
            
            if (timer > 1.0f) {
                timer = 0;
                state = 1;
            }
        }

        else if (state == 1) {
           
            float t = std::min(timer / 2.0f, 1.0f);
            float angle = t * 45.0f;

            Mat3 R = rotation(angle);
            P_rot = multiply(R, P);

            if (t >= 1.0f) {
                timer = 0;
                state = 2;
            }
        }

        else if (state == 2) {
           
            float t = std::min(timer / 2.0f, 1.0f);

            Mat3 R = rotation(45.0f);
            P_rot = multiply(R, P);

            Mat3 T = translation(t * 1.0f, t * 2.0f);
            P_final = multiply(T, P_rot);

            if (t >= 1.0f) {
                timer = 0;
                state = 3;
            }
        }

        else if (state == 3) {
           
            timer = 0;
            state = 0;
        }


    
        glLineWidth(1.0f);
        glColor3f(0.3f, 0.3f, 0.3f);

        glBegin(GL_LINES);
        for (int x = -5; x <= 5; x++) {
            glVertex2f(x, -5);
            glVertex2f(x, 5);
        }
        for (int y = -5; y <= 5; y++) {
            glVertex2f(-5, y);
            glVertex2f(5, y);
        }
        glEnd();

        glLineWidth(2.0f);

        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        glVertex2f(-5, 0);
        glVertex2f(5, 0);
        glEnd();

        glColor3f(0, 1, 0);
        glBegin(GL_LINES);
        glVertex2f(0, -5);
        glVertex2f(0, 5);
        glEnd();

        glColor3f(1, 0.5f, 0);
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(P.x, P.y);
        glEnd();

        glColor3f(0, 0.7f, 1);
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(P_rot.x, P_rot.y);
        glEnd();

        glColor3f(0, 1, 0.8f);
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(P_final.x, P_final.y);
        glEnd();

        glPointSize(10.0f);
        glBegin(GL_POINTS);

        glColor3f(1, 0, 0);
        glVertex2f(P.x, P.y);

        glColor3f(0, 0.7f, 1);
        glVertex2f(P_rot.x, P_rot.y);

        glColor3f(0, 1, 0.8f);
        glVertex2f(P_final.x, P_final.y);

        glEnd();

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
