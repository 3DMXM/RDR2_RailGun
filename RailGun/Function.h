#pragma once
#include "inc/natives.h"
#include <time.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "scriptmenu.h"
#include "json/json/json.h"
using namespace std;

string GT(const string& strGBK);//GBK编码转换到UTF8编码
void createPrompt(int* prompt, int button, string text, int secondButton = 0);	//创建提示
void setPromptEnabled(int* prompt, bool enabled);	// 设置提示启用
Vector3 GET_COORDS_INFRONT(float d, float offset = 0.0F);	//获取玩家镜头朝向位置
void DrawGameRect(float x, float y, float width, float height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);	//绘制背景
void drawBoxOutline(float x, float y, float radius, float thickness, int r, int g, int b, int a);	//绘制正方形边框
void drawBoxOutline(float x, float y, float x_radius, float y_radius, float thickness, int r, int g, int b, int a);	//绘制长方形边框
Vector3 GET_COORDS_INFRONT_OF_ANGLE(Vector3 ro, float d, float offset = 0.0F);	//获取角度的信息

void drawRedboxes();	//显示NPC位置
void drawGreenBoxesForPlayer();	//显示玩家的位置
float clamp360(float value);

namespace camShit {
	int cam = 0;
	Vector3 location = { 0,0,0 };
	Vector3 rotation = { 0,0,0 };//GET_GAMEPLAY_CAM_ROT(2);
	Vector3 velocityForFocus = { 0, 0, 0 };
	int entity = 0;
	bool doCamRot = true;
	bool doCamPos = true;
	bool doFocus = true;
	bool doLookat = true;
	void loop();
	void attachToLocation(float x, float y, float z, bool setFocusOnArea = true);
	void resetCam(bool clearFocus = true);
}
Vector3 GetCoordsInfrontOfCam(float distance);
float getHeightAboveGround(Vector3 position);	// 获取地面坐标
void playAudio();	// 播放声音
string CheckUpdates();	//检测更新