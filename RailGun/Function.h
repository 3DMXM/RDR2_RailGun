#pragma once
#include "inc/natives.h"
#include <time.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "scriptmenu.h"
#include "json/json/json.h"
using namespace std;

string GT(const string& strGBK);//GBK����ת����UTF8����
void createPrompt(int* prompt, int button, string text, int secondButton = 0);	//������ʾ
void setPromptEnabled(int* prompt, bool enabled);	// ������ʾ����
Vector3 GET_COORDS_INFRONT(float d, float offset = 0.0F);	//��ȡ��Ҿ�ͷ����λ��
void DrawGameRect(float x, float y, float width, float height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);	//���Ʊ���
void drawBoxOutline(float x, float y, float radius, float thickness, int r, int g, int b, int a);	//���������α߿�
void drawBoxOutline(float x, float y, float x_radius, float y_radius, float thickness, int r, int g, int b, int a);	//���Ƴ����α߿�
Vector3 GET_COORDS_INFRONT_OF_ANGLE(Vector3 ro, float d, float offset = 0.0F);	//��ȡ�Ƕȵ���Ϣ

void drawRedboxes();	//��ʾNPCλ��
void drawGreenBoxesForPlayer();	//��ʾ��ҵ�λ��
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
float getHeightAboveGround(Vector3 position);	// ��ȡ��������
void playAudio();	// ��������
string CheckUpdates();	//������