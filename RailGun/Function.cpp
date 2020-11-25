#include "Function.h"
#include <functional>
#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <atlstr.h>
#include <wininet.h>
#pragma  comment(lib, "wininet.lib")

using namespace std;

//GBK编码转换到UTF8编码
string GT(const string& strGBK)
{
	string strOutUTF8 = "";
	WCHAR* str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n); n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char* str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

// 判断是否有效
bool isPromptValid(int prompt) 
{
	return UI::_0x347469FBDD1589A9(prompt);
}
// 创建提示
void createPrompt(int* prompt, int button, string text, int secondButton) 
{
	if (!isPromptValid(*prompt)) {
		*prompt = UI::_0x04F97DE45A519419();//_PROMPT_REGISTER_BEGIN()

		UI::_0xB5352B7494A08258(*prompt, button);
		if (secondButton != 0)
			UI::_0xB5352B7494A08258(*prompt, secondButton);

		UI::_0x71215ACCFDE075EE(*prompt, false);//_PROMPT_SET_VISIBLE	提示设置可见
		UI::_0x8A0FB4D03A630D21(*prompt, false);//_PROMPT_SET_ENABLED	启用提示集
		UI::_0xF7AA2696A22AD8B9(*prompt);
	}
	//必要时更新文字
	UI::_0x5DD02A8318420DD7(*prompt, GAMEPLAY::CREATE_STRING(10, "LITERAL_STRING", (char*)text.c_str())/*"IB_SELECT"*/);//_PROMPT_SET_TEXT
}
// 设置提示启用
void setPromptEnabled(int* prompt, bool enabled) 
{
	if (isPromptValid(*prompt)) {
		
		UI::_0x8A0FB4D03A630D21(*prompt, enabled);//enable
		//UI::_0x06565032897BA861(*prompt);//enabled this frame
		UI::_0x71215ACCFDE075EE(*prompt, enabled);//visible
	}
}

const double PI = 3.141592653589793238463;
// 获取玩家镜头朝向位置
Vector3 GET_COORDS_INFRONT(float d, float offset) 
{
	Vector3 ro = CAM::GET_GAMEPLAY_CAM_ROT(2);
	Vector3 rot;
	float x = ro.z + 90 + offset;
	while (x < 0) x = x + 360;
	while (x > 360) {
		x = x - 360;
	}
	bool fix = false;
	if (x > 180) {
		fix = true;
	}
	rot.x = x;
	rot.z = ro.x;
	float cosrotz = cos(rot.z * PI / 180.0);
	float x1 = 0;
	float y1 = 0;
	float z1 = d * sin(rot.z * PI / 180.0);
	if (fix == true) {
		rot.x = (rot.x - 180);
		x1 = -d * cos(rot.x * PI / 180.0) * cosrotz;
		y1 = -d * sin(rot.x * PI / 180.0) * cosrotz;
	}
	else {
		x1 = d * cos(rot.x * PI / 180.0) * cosrotz;
		y1 = d * sin(rot.x * PI / 180.0) * cosrotz;
	}
	Vector3 Coords = { x1,y1,z1 };
	return Coords;
}


//绘制UI 背景颜色
void DrawGameRect(float x, float y, float width, float height, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	GRAPHICS::DRAW_RECT(x, y, width, height, r, g, b, a, true, true/*confused screaming*/);
}
// 绘制UI 边框
void drawBoxOutline(float x, float y, float radius, float thickness, int r, int g, int b, int a) 
{
	int rx, ry;//originally float but int does make more sense...
	GRAPHICS::GET_SCREEN_RESOLUTION(&rx, &ry);
	float aspectRatio = (float)ry / (float)rx;//want y to be the standard

	float xLeft = x - radius * aspectRatio;
	float xRight = x + radius * aspectRatio;
	float yTop = y - radius;
	float yBottom = y + radius;
	float length = radius * 2;
	DrawGameRect(xLeft + radius * aspectRatio, yTop + thickness / 2.0f, length * aspectRatio, thickness, r, g, b, a);//top bar
	DrawGameRect(xLeft + radius * aspectRatio, yBottom - thickness / 2.0f, length * aspectRatio, thickness, r, g, b, a);//bottom bar
	DrawGameRect(xLeft + thickness * aspectRatio / 2.0f, yTop + radius, thickness * aspectRatio, length, r, g, b, a);//left bar
	DrawGameRect(xRight - thickness * aspectRatio / 2.0f, yTop + radius, thickness * aspectRatio, length, r, g, b, a);//right bar
}
void drawBoxOutline(float x, float y, float x_radius, float y_radius, float thickness, int r, int g, int b, int a)
{
	int rx, ry;//originally float but int does make more sense...
	GRAPHICS::GET_SCREEN_RESOLUTION(&rx, &ry);
	float aspectRatio = (float)ry / (float)rx;//want y to be the standard

	float xLeft = x - x_radius * aspectRatio;
	float xRight = x + x_radius * aspectRatio;
	float yTop = y - y_radius;
	float yBottom = y + y_radius;

	float length = x_radius * 2;
	DrawGameRect(xLeft + x_radius * aspectRatio, yTop + thickness / 2.0f, length * aspectRatio, thickness, r, g, b, a);//top bar
	DrawGameRect(xLeft + x_radius * aspectRatio, yBottom - thickness / 2.0f, length * aspectRatio, thickness, r, g, b, a);//bottom bar
	DrawGameRect(xLeft + thickness * aspectRatio / 2.0f, yTop + y_radius, thickness * aspectRatio, length, r, g, b, a);//left bar
	DrawGameRect(xRight - thickness * aspectRatio / 2.0f, yTop + y_radius, thickness * aspectRatio, length, r, g, b, a);//right bar
}

#define nearbyPedArraySize 10000
struct nearbyEnts {
	int size;
	int64_t entities[nearbyPedArraySize];
};
// 扫描所有NPC
void runOnAllNearbyPedsToPlayer(std::function<void(int)> callback) {
	nearbyEnts arr;
	arr.size = nearbyPedArraySize;
	int ped = PLAYER::PLAYER_PED_ID();
	int size = PED::GET_PED_NEARBY_PEDS(ped, (int*)&arr, ped, 0/*unk*/);
	for (int i = 0; i < size; i++) {
		callback(arr.entities[i]);
	}
}
// 扫描所有载具和马匹
void runOnAllNearbyVehiclesToPlayer(std::function<void(int)> callback) {
	nearbyEnts arr;
	arr.size = nearbyPedArraySize;
	int ped = PLAYER::PLAYER_PED_ID();
	int size = PED::GET_PED_NEARBY_VEHICLES(ped, (int*)&arr);
	for (int i = 0; i < size; i++) {
		callback(arr.entities[i]);
	}
}
void runOnAllNearbyPedsAndVehiclesToPlayer(std::function<void(int)> callback) {
	runOnAllNearbyPedsToPlayer(callback);
	runOnAllNearbyVehiclesToPlayer(callback);
}
// 显示NPC位置
void drawRedboxes() {
	runOnAllNearbyPedsAndVehiclesToPlayer([](auto ped) {
		//GET_SCREEN_COORD_FROM_WORLD_COORD
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(ped, false, false/*idk*/);	// 获取ped坐标
		float x, y;
		GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(pos.x, pos.y, pos.z, &x, &y);	//全局获取坐标位置
		drawBoxOutline(x, y, 0.01f, 0.002f, 255, 0, 0, 255);	// 绘制红框
		});
}
// 显示玩家的位置
void drawGreenBoxesForPlayer()
{
	//Entity ped = PLAYER::PLAYER_PED_ID();
	//Vector3 pos = ENTITY::GET_ENTITY_COORDS(ped, false, false/*idk*/);	// 获取ped坐标
	float x, y;
	Vector3 pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 3.0, -0.3);
	GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(pos.x, pos.y, pos.z, &x, &y);	//全局获取坐标位置
	drawBoxOutline(x, y, 0.01f, 0.002f, 0, 255, 0, 255);	// 绘制绿框
}

float clamp360(float value) {
	while (value < 0)
		value += 360;
	return value;
}
namespace camShit {	
	void loop() {
		if (CAM::DOES_CAM_EXIST(cam)) {
			if (doCamRot)
				CAM::SET_CAM_ROT(cam, rotation.x, rotation.y, rotation.z, 2);	//
			if (doCamPos)
				CAM::SET_CAM_COORD(cam, location.x, location.y, location.z);	//设置镜头坐标
			if (doFocus)
				STREAMING::SET_FOCUS_POS_AND_VEL(location.x, location.y, location.z, velocityForFocus.x, velocityForFocus.y, velocityForFocus.z);	// 设置新焦点
			if (doLookat) {
				CAM::POINT_CAM_AT_COORD(cam, rotation.x, rotation.y, rotation.z);	//将镜头指向坐标
				Vector3 r = CAM::GET_CAM_ROT(cam, 2);	//获取镜头焦点
				r.x = clamp360(r.x);
				r.y = clamp360(r.y);
				r.z = clamp360(r.z);
				CAM::SET_CAM_ROT(cam, r.x, 0/*r.y*/, r.z, 2);
			}
		}
	}

	void attachToLocation(float x, float y, float z, bool setFocusOnArea) {
		doCamRot = true;//no need to set rot... we are doing lookat
		doCamPos = true;
		doFocus = true;
		doLookat = false;
		if (setFocusOnArea) {
			STREAMING::CLEAR_FOCUS();	//清除聚焦
			STREAMING::SET_FOCUS_POS_AND_VEL(x, y, z, 0, 0, 0);//should be right I think	设置新焦点
		}
		entity = 0;
		if (!CAM::DOES_CAM_EXIST(cam))	//摄像机是否存在
			cam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);	//创建摄像机
		CAM::SET_CAM_COORD(cam, x, y, z);	//设置摄像机坐标
		CAM::SET_CAM_ACTIVE(cam, 1);	//激活摄像机
		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0, false/*idk*/);	//渲染摄像机
	}

	void resetCam(bool clearFocus) {
		if (CAM::DOES_CAM_EXIST(cam)) {
			if (clearFocus)
				STREAMING::CLEAR_FOCUS();
			CAM::SET_CAM_ACTIVE(cam, 0);
			CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 0, false/*idk*/);
			CAM::DESTROY_CAM(cam, 0);
			cam = 0;
		}
		cam = 0;
	}
	bool enabled() {

		return CAM::DOES_CAM_EXIST(cam);
	}
}



Vector3 GET_COORDS_INFRONT_OF_ANGLE(Vector3 ro, float d, float offset) {	
	Vector3 rot;
	float x = ro.z + 90 + offset;
	while (x < 0) x = x + 360;
	while (x > 360) {
		x = x - 360;
	}
	bool fix = false;
	if (x > 180) {
		fix = true;
	}
	rot.x = x;
	rot.z = ro.x;
	float cosrotz = cos(rot.z * PI / 180.0);
	float x1 = 0;
	float y1 = 0;
	float z1 = d * sin(rot.z * PI / 180.0);
	if (fix == true) {
		rot.x = (rot.x - 180);
		x1 = -d * cos(rot.x * PI / 180.0) * cosrotz;
		y1 = -d * sin(rot.x * PI / 180.0) * cosrotz;
	}
	else {
		x1 = d * cos(rot.x * PI / 180.0) * cosrotz;
		y1 = d * sin(rot.x * PI / 180.0) * cosrotz;
	}
	Vector3 Coords = { x1,y1,z1 };
	return Coords;
}

//获取玩家准心周围范围区域坐标 
Vector3 GetCoordsInfrontOfCam(float distance)
{
	Vector3 GameplayCamRot = CAM::GET_GAMEPLAY_CAM_ROT(2);	//获取玩家镜头角度
	Vector3 GameplayCamCoord = CAM::GET_GAMEPLAY_CAM_COORD();	//获取玩家镜头坐标

	float tan = cos(GameplayCamRot.x * PI / 180.0) * distance;
	float xPlane = sin(GameplayCamRot.z * PI / 180.0 * -1.0f) * tan + GameplayCamCoord.x;
	float yPlane = cos(GameplayCamRot.z * PI / 180.0 * -1.0f) * tan + GameplayCamCoord.y;
	float zPlane = sin(GameplayCamRot.x * PI / 180.0) * distance + GameplayCamCoord.z;

	Vector3 ret = { xPlane, yPlane, zPlane };

	return ret;
}

// 获取地面Z轴
float getHeightAboveGround(Vector3 position)
{
	float groundZ = 0;
	GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(position.x, position.y, 1000.0f, &groundZ, true);
	return groundZ;
}
//
//void playAudio()
//{
//	AUDIO::STOP_SOUND_FRONTEND("NAV_RIGHT", "HUD_SHOP_SOUNDSET");
//	AUDIO::PLAY_SOUND_FRONTEND("NAV_RIGHT", "HUD_SHOP_SOUNDSET", 1, 0);
//
//}

//对网页进行转码
wchar_t* Convert(CString str, int targetCodePage)
{

	int iunicodeLen = MultiByteToWideChar(targetCodePage, 0, (LPCCH)str.GetBuffer(), -1, NULL, 0);
	wchar_t* pUnicode = NULL;
	pUnicode = new wchar_t[iunicodeLen + 1];
	memset(pUnicode, 0, (iunicodeLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(targetCodePage, 0, (LPCCH)str.GetBuffer(), -1, (LPWSTR)pUnicode, iunicodeLen);//映射一个字符串到一个款字节中
	return pUnicode;
}
/*获取网页的源码，参数1为网页链接，2为缓冲区指针*/
bool GetHtml(LPCTSTR szURL, CString& getbuf)
{
	HINTERNET	hInternet, hUrl;
	char		buffer[1124];
	WCHAR		wBuffer[1124];

	DWORD		dwBytesRead = 0;
	DWORD		dwBytesWritten = 0;
	BOOL		bIsFirstPacket = true;
	BOOL		bRet = true;
	int			nNowcopyDate = 0;

	hInternet = InternetOpen(_T("Mozilla/4.0 (compatible)"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, INTERNET_INVALID_PORT_NUMBER, 0);//初始化应用程序，使用WinNet
	if (hInternet == NULL)
		return FALSE;

	hUrl = InternetOpenUrl(hInternet, szURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);//打开一个资源 ftp，gopher，http开头

	if (hUrl == NULL)
	{
		DWORD m = GetLastError();
		return FALSE;
	}
	do
	{
		memset(buffer, 0, sizeof(char) * 1124);
		InternetReadFile(hUrl, buffer, sizeof(char) * 1024, &dwBytesRead);
		bIsFirstPacket = false;
		nNowcopyDate = +dwBytesRead;
		wchar_t* punicode;
		punicode = Convert(buffer, CP_UTF8);// 对源码进行转码   第二个参数为网页的编码格式

		//CString strTmp = CString(buffer);	

		getbuf += _T("\r\n");
		getbuf += punicode;
		delete punicode;
	} while (dwBytesRead > 0);

	InternetCloseHandle(hUrl);
	InternetCloseHandle(hInternet);
	return TRUE;
}
// 检测更新
string CheckUpdates()
{
	double nowV = 0.1;
	std::stringstream str;
	str << std::setprecision(3) << nowV;
	string v = "当前版本 v" + str.str() + " By:小莫 ";
	try
	{
		Json::Reader reader;
		Json::Value root;
		//json js;
		CString  retbuf;
		if (GetHtml(TEXT("https://mod.3dmgame.com/mod/API/170521"), retbuf))	//是否成功读取，将值写入retbuf
		{
			//retbuf值：
			//{"id":"170521","mods_version":"0.1","mods_author":"◕小莫◕","mods_click_cnt":2,"mods_download_cnt":0,"mods_mark_cnt":0,"mods_collection_cnt":0,"mods_updateTime":"2020-11-25 13:35:23"}
			if (reader.parse((string)retbuf, root)) {
				//js = json::parse((string)retbuf);	//str to json
				string newVs = root["mods_version"].asString();	//获取版本
				double newVf = stof(newVs);			//转为 double
				const double eps = 1e-6;//double 判断大小需要对大的数 - eps 或对小的数 + eps
				// 判断是否有更新
				if (newVf - eps > nowV)		return v + "[有更新可用]";
				else						return v + "[已是最新版本]";
			}
		}
		else { return v + "[检测更新失败]"; }
	}
	catch (const std::exception&) { return  v + "[检测更新失败]"; }
}