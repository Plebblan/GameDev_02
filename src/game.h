#pragma once

#ifndef GAME_H
#define GAME_H

#define PI 3.14

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <string>
#include <cmath>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

enum class AttackDirection
{
    None,
    Left,
    Right,
    Up,
    Down
};

class GameWindow{
    private:
        int m_width;
        int m_height;
    public:
        SDL_Window *window;
        SDL_Renderer* renderer;
        SDL_Event event;
        GameWindow();
        ~GameWindow();
        int init(std::string title, int width = WINDOW_WIDTH, int height = WINDOW_HEIGHT);
        void draw();
        void clear();
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
};

class Vector2 {
	public:
		float x, y;
		Vector2();
		Vector2(float x, float y);
		void sub(Vector2 v);
		void sub(float x, float y);
		Vector2 sub_N(Vector2 v);
		Vector2 sub_N(float f);
		void add(Vector2 v);
		void add(float x, float y);
		Vector2 add_N(Vector2 v);
		Vector2 add_N(float f);
		void mul(float f);
		void e_mul(Vector2 v);
		float dot(Vector2 v);
		Vector2 mul_N(float f);
		Vector2 mul_N(Vector2 v);
		void normalize();
		float length();
		double angle(Vector2 v);
		void set(float x, float y);
		void set(Vector2 vec);
		void fromAngle(double angle);
		void rotate(double angle);
};

#endif