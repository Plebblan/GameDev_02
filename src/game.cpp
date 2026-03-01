#include "game.h"

double round_3dec(double val) {
	return round(val * 100.0 ) / 100.0;
}

//====================Vec2====================
Vector2::Vector2() {
	this->x = this->y = 0.0;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

void Vector2::add(Vector2 v) {
	this->x += v.x;
	this->y += v.y;
}

void Vector2::add(float x, float y) {
	this->x += x;
	this->y += y;
}

Vector2 Vector2::add_N(Vector2 v){
	Vector2 ret;
	ret.x = this->x + v.x;
	ret.y = this->y + v.y;
	return ret;
}

Vector2 Vector2::add_N(float f){
	Vector2 ret;
	ret.x = this->x + f;
	ret.y = this->y + f;
	return ret;
}

void Vector2::sub(Vector2 v) {
	this->x -= v.x;
	this->y -= v.y;
}

void Vector2::sub(float x, float y) {
	this->x -= x;
	this->y -= y;
}

Vector2 Vector2::sub_N(Vector2 v){
	Vector2 ret;
	ret.x = this->x - v.x;
	ret.y = this->y - v.y;
	return ret;
}

Vector2 Vector2::sub_N(float f){
	Vector2 ret;
	ret.x = this->x - f;
	ret.y = this->y - f;
	return ret;
}

void Vector2::mul(float f) {
	this->x *= f;
	this->y *= f;
}

void Vector2::e_mul(Vector2 v){
	this->x *= v.x;
	this->y *= v.y;
}

float Vector2::dot(Vector2 v){
	return this->x * v.x + this->y * v.y;
}

Vector2 Vector2::mul_N(float f) {
	Vector2 ret;
	ret.x = this->x*f;
	ret.y = this->y*f;
	return ret;
}

Vector2 Vector2::mul_N(Vector2 v) {
	Vector2 ret;
	ret.x = this->x*v.x;
	ret.y = this->y*v.y;
	return ret;
}

float Vector2::length() {
	return sqrt(x*x+y*y);
}

void Vector2::normalize() {
	float l = length();
	x = x/l;
	y = y/l;
}

double Vector2::angle(Vector2 v) { //Note that this function returns DEG
	double dot = this->x*v.x + this->y*v.y;
	double det = this->x*v.y - this->y*v.x;
	double ret = atan2(det, dot);
	ret *= (180.0/PI);
	while(ret<0) ret+=360;
	while(ret>360) ret-=360;
	return ret;
}

void Vector2::set(float x, float y) {
	this->x = x;
	this->y = y;
}

void Vector2::set(Vector2 vec){
    this->x = vec.x;
    this->y = vec.y;
}

void Vector2::fromAngle(double angle) { //Note that this function takes DEG arguments
	this->x = round_3dec(sin(angle*(PI/180.0)));
	this->y = round_3dec(cos((180-angle)*(PI/180.0))); //I honestly have no clue why I have to do 180-angle here, but it only works that way :D
	this->normalize();
}

void Vector2::rotate(double angle) { //takes deg argument
	angle = angle*(PI/180.0);
	float newX, newY;
    newX = x * cos(angle) - y * sin(angle);
    newY = x * sin(angle) + y * cos(angle);
    x = newX;
    y = newY;
}

//====================GameWindow====================
GameWindow::GameWindow(){
    this->window = NULL;
    this->renderer = NULL;
}

GameWindow::~GameWindow(){
    SDL_DestroyWindow(this->window);
    SDL_DestroyRenderer(this->renderer);
}

int GameWindow::init(std::string title, int width, int height){
    this->window = SDL_CreateWindow(title.c_str(), width, height, 0);
    if (this->window == NULL){
        return -1;
    }
    if (!SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)){
        return -1;
    }

    this->renderer = SDL_CreateRenderer(this->window, NULL);
    if (this->renderer == NULL){
        return -1;
    }

    return 0;
}

