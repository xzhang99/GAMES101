// GAMES101 HW0

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#include <cmath>
#include <iostream>
#include <array>
#include <algorithm>
    #include <eigen3/Eigen/Dense> // Use Eigen/Dense for Matrix4f
#include <vector>

constexpr float MY_PI = 3.14159265358979323846f;

Eigen::Matrix4f get_model_matrix(float rotation_angle) // Make function static
{
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity ();

	float radian = rotation_angle * MY_PI / 180.0f;

	model << cos (radian), -sin (radian), 0, 0,
			 sin (radian),  cos (radian), 0, 0,
			 0,             0,            1, 0,
		0, 0, 0, 1;
	return model;
}
Eigen::Matrix4f get_view_matrix (Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity ();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0],
        0, 1, 0, -eye_pos[1],
        0, 0, 1, -eye_pos[2],
        0, 0, 0, 1;
    view = translate * view;

    return view;
}

Eigen::Matrix4f get_projection_matrix (float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity ();

	float radian = eye_fov * MY_PI / 180.0f;
	float t = tan (radian / 2) * zNear;
	float b = -t;
	float r = aspect_ratio * t;
	float l = -r;

	//orthographic projection
	Eigen::Matrix4f M_ortho_scale = Eigen::Matrix4f::Identity ();

	M_ortho_scale << 2 / (r - l), 0, 0, 0,
					 0, 2 / (t - b), 0, 0,
					 0, 0, 2 / (zNear - zFar), 0,
		0, 0, 0, 1;

	Eigen::Matrix4f M_ortho_trans = Eigen::Matrix4f::Identity ();
	M_ortho_trans << 1, 0, 0, -(r + l) / 2,
					 0, 1, 0, -(t + b) / 2,
					 0, 0, 1, -(zNear + zFar) / 2,
		0, 0, 0, 1;

	Eigen::Matrix4f M_ortho = M_ortho_scale * M_ortho_trans;

	Eigen::Matrix4f M_p2o = Eigen::Matrix4f::Zero ();
	M_p2o << zNear, 0, 0, 0,
			 0, zNear, 0, 0,
			 0, 0, zNear + zFar, -zNear * zFar,
		0, 0, 1, 0;

	projection = M_ortho * M_p2o;

	return projection;
}

int main (int argc, char* argv[])
{
    float angle = 0;
    bool quit = false;
    SDL_Event e;

    const int width = 700;
    const int height = 700;

    if (SDL_Init (SDL_INIT_VIDEO) < 0) return -1;

    SDL_Window* window = SDL_CreateWindow ("GAMES101 HW1 - Triangle Rotation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);

    std::vector<Eigen::Vector4f> v = {
        {2.0, 0.0, -2.0, 1.0},
        {0.0, 2.0, -2.0, 1.0},
        {-2.0, 0.0, -2.0, 1.0}
    };

    while (!quit) {
        while (SDL_PollEvent (&e)) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_a) angle += 10; // 按 A 键增加旋转角
                if (e.key.keysym.sym == SDLK_d) angle -= 10; // 按 D 键减少旋转角
                if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
            }
        }

        SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
        SDL_RenderClear (renderer);

        Eigen::Matrix4f model = get_model_matrix (angle);
        Eigen::Matrix4f view = get_view_matrix ({ 0, 0, 5 });
        Eigen::Matrix4f projection = get_projection_matrix (45, 1, 0.1, 50);

   
        Eigen::Matrix4f mvp = projection * view * model;

        std::vector<SDL_FPoint> screen_pts;
        for (auto& vert : v) {
            
            Eigen::Vector4f v_clip = mvp * vert;

           
            float w = v_clip.w ();
            Eigen::Vector3f v_ndc = { v_clip.x () / w, v_clip.y () / w, v_clip.z () / w };

            
            float sx = (v_ndc.x () + 1.0f) * 0.5f * width;
            float sy = (1.0f - v_ndc.y ()) * 0.5f * height; // 翻转 Y

            screen_pts.push_back ({ sx, sy });
        }

        SDL_SetRenderDrawColor (renderer, 0, 0, 255, 255); // 蓝色线框

        // 连接第一个点和第二个点
        SDL_RenderDrawLineF (renderer, screen_pts[0].x, screen_pts[0].y, screen_pts[1].x, screen_pts[1].y);
        SDL_RenderDrawLineF (renderer, screen_pts[1].x, screen_pts[1].y, screen_pts[2].x, screen_pts[2].y);
        SDL_RenderDrawLineF (renderer, screen_pts[2].x, screen_pts[2].y, screen_pts[0].x, screen_pts[0].y);

        SDL_RenderPresent (renderer);
        SDL_Delay (10); 
    }

    SDL_DestroyRenderer (renderer);
    SDL_DestroyWindow (window);
    SDL_Quit ();
    return 0;
}