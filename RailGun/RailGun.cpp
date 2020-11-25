#include "RailGun.h"
#include <atlbase.h>



// 全局变量 π
const double PI = 3.141592653589793238463;
// 开启天基轨道炮
class SpawnDriveableUFO : public MenuItemSwitchable
{

	bool missileActive;
	Vector3 location = { 0,0,0 };
	Vector3 rotation = { 0,0,0 };
	Vector3 velocity = { 0,0,0 };
	float dropSpeed = 0.0f;	//下坠速度
	float Speed = 1.0f;	//移动速度
	float initialZForGroundTest = 0;

	virtual void OnSelect()
	{
		//创建右下角提示按钮
		static int LensMoveUD = -1;
		static int LensMoveLR = -1;
		static int LensMoveEnlarge = -1;
		static int LensMoveShrink = -1;
		static int LensMoveFire = -1;

		createPrompt(&LensMoveUD, INPUT_MOVE_UD, GT("上下移动"));
		createPrompt(&LensMoveLR, INPUT_MOVE_LR, GT("左右移动"));
		createPrompt(&LensMoveEnlarge, Key_Shift, GT("镜头放大"));
		createPrompt(&LensMoveShrink, INPUT_DUCK, GT("镜头缩小"));
		createPrompt(&LensMoveFire, INPUT_JUMP, GT("发射"));

		if (GetState())
		{
			setPromptEnabled(&LensMoveUD, false);
			setPromptEnabled(&LensMoveLR, false);
			setPromptEnabled(&LensMoveEnlarge, false);
			setPromptEnabled(&LensMoveShrink, false);
			setPromptEnabled(&LensMoveFire, false);
			camShit::resetCam();
			ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), false);
		}
		else
		{
			Ped myPed = PLAYER::GET_PLAYER_PED(0);
			Vector3 myPos = ENTITY::GET_ENTITY_COORDS(myPed, false, false/*idk*/);
			location.x = myPos.x;
			location.y = myPos.y;
			location.z = myPos.z + 100;
			camShit::attachToLocation(location.x, location.y, location.z);
			ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), true);

			setPromptEnabled(&LensMoveUD, true);
			setPromptEnabled(&LensMoveLR, true);
			setPromptEnabled(&LensMoveEnlarge, true);
			setPromptEnabled(&LensMoveShrink, true);
			setPromptEnabled(&LensMoveFire, true);

			CONTROLS::DISABLE_CONTROL_ACTION(1, INPUT_DUCK, false);	//在天上时禁用玩家操控角色
		}
		SetState(!GetState());
	}

	virtual void OnFrame()
	{
		if (GetState())
		{
			// 绘制导弹UI
			DrawGameRect(0.5f, 0.5f, 1.0f, 1.0f, 56, 173, 169, 150);//绘制背景 rgb(47, 53, 66)
			drawBoxOutline(0.5f, 0.5f, 0.001f, 0.005f, 255, 255, 255, 255);	// 绘制中心点
			drawBoxOutline(0.5f, 0.5f, 0.05f, 0.005f, 255, 255, 255, 255);	//绘制中心方块
			drawBoxOutline(0.5f, 0.5f, 0.65f, 0.3f, 0.005f, 255, 255, 255, 255);	//绘制大方块
			drawRedboxes();	//显示NPC的位置
			drawGreenBoxesForPlayer(); //显示玩家的位置

			Vector3 velocityFinal = GET_COORDS_INFRONT_OF_ANGLE(rotation, dropSpeed);
			/*
			  x轴 + : 右移动
			  x轴 - : 左移动
			  z轴 + : 上移动
			  z轴 - : 下移动
			  y轴 + : 放大
			  y轴 - : 缩小
			*/
			if (IsKeyDownLong(87))	velocityFinal.z += Speed; playAudio();	//W
			if (IsKeyDownLong(83))	velocityFinal.z -= Speed;	//S
			if (IsKeyDownLong(65))	velocityFinal.x -= Speed;	//A
			if (IsKeyDownLong(68))	velocityFinal.x += Speed;	//D
			if (IsKeyDownLong(17))	velocityFinal.y -= Speed;	//Ctrl
			if (IsKeyDownLong(16))	velocityFinal.y += Speed;	//shift
						
			//velocityFinal.z = 1000;
			location.x += velocityFinal.x;
			location.y += velocityFinal.z;
			location.z += -velocityFinal.y;

			float onZ = getHeightAboveGround(location);

			if (location.z <= onZ + 30) location.z = onZ + 30;	// 判断是否已穿过地面了

			if (IsKeyDown(32)) //space
			{
				// 发射....
				//Vector3 coords = GetCoordsInfrontOfCam(0.1f);
				Vector3 coords = location;
				coords.z = onZ;
				FIRE::ADD_OWNED_EXPLOSION(PLAYER::PLAYER_PED_ID(), coords.x, coords.y, coords.z, 11, 1000, true, false, 10);
				//SetStatusText(GT("地面" + to_string(location.z) + "." + to_string(onZ)));
				//SetStatusText(GT("爆炸坐标：" + to_string(coords.x) + to_string(coords.y) + to_string(coords.z)));
			}

			//GRAPHICS::DRAW_LIGHT_WITH_RANGE(location.x, location.y, onZ, 255, 0, 0, 1000, 10000);	//绘制激光


			camShit::velocityForFocus.x = velocityFinal.x;
			camShit::velocityForFocus.y = velocityFinal.y;
			camShit::velocityForFocus.z = velocityFinal.z;
			camShit::location.x = location.x;
			camShit::location.y = location.y;
			camShit::location.z = location.z;
			camShit::rotation.x = clamp360(-90.0f);//pitch
			camShit::rotation.y = 0;//roll
			//camShit::rotation.z = clamp360(randomFloatInRange(-1, 1));//yaw

			float groundZ = 0;
			GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, initialZForGroundTest, &groundZ, true);//true 使镜头对准地面
			camShit::loop();
		}
	}

public:
	SpawnDriveableUFO(string caption)
		: MenuItemSwitchable(caption) {}
};
// 范围扫荡
class MenuItemRangeSweep : public MenuItemSwitchable
{	
	virtual void OnFrame()
	{
		if (GetState())
		{
			drawBoxOutline(0.5f, 0.5f, 0.001f, 0.005f, 255, 255, 255, 255);	// 绘制中心点
			Vector3 coords = GetCoordsInfrontOfCam(40);
			FIRE::ADD_OWNED_EXPLOSION(PLAYER::PLAYER_PED_ID(), coords.x, coords.y, coords.z, 11, 1000, true, false, 1);

			//static int Timeout = 0;
			//Vector3 FromCoord = PED::GET_PED_BONE_COORDS(PLAYER::PLAYER_PED_ID(), 57005, 0, 0, 0);//probably a bad bone index
			//Hash WeaponID = GAMEPLAY::GET_HASH_KEY("WEAPON_RIFLE_SPRINGFIELD");//w_rifle_boltaction01 /*GAMEPLAY::GET_HASH_KEY("WEAPON_RIFLE_SPRINGFIELD")*//*this weapon makes a better noise*/
			//WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &WeaponID, 0, 0, 0);
			//GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(FromCoord.x, FromCoord.y, FromCoord.z, ToCoord.x, ToCoord.y, ToCoord.z, 250, 0, WeaponID, PLAYER::PLAYER_PED_ID(), 1, 0, 1, true/*idk*/);
			//Timeout = GAMEPLAY::GET_GAME_TIMER() + 50;
		}
	}
public:
	MenuItemRangeSweep(string caption)
		: MenuItemSwitchable(caption) {}
};
//// 测试按钮
//class MenuItemTest :public MenuItemDefault
//{
//	bool enabled = false;
//	Vector3 position = { 0,0,0 };
//	Vector3 velocity = { 0,0,0 };
//	float minHeight = 0.0f;
//	float maxHeightToLane = 20.0f;
//	float slowModeHeight = 40.0f;
//
//	// 获取地面Z轴
//	float getHeightAboveGround() {
//		float groundZ = 0;
//		GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(position.x, position.y, 1000.0f, &groundZ, true);
//		return position.z - groundZ;
//	}
//
//	virtual void OnSelect()
//	{
//		//SetStatusText(GT("测试"));
//		//创建右下角提示按钮
//		//int promptShift = -1;
//		//int promptF = -1;
//		//bool controllerMode = false;
//		//createPrompt(&promptShift, controllerMode ? Buttons::Button_L1 : ButtonHashKey::Key_Shift, GT("点击Shift"));
//		//createPrompt(&promptF, controllerMode ? Buttons::Button_Triangle : ButtonHashKey::Key_F, GT("进入UFO"));//Changed to F    //enter  1138488863 0x43DBF61F
//		//setPromptEnabled(&promptShift, true);
//		//setPromptEnabled(&promptF, true);
//
//		//创建UFO
//		int ufoID = -1;
//		Ped ped = PLAYER::PLAYER_PED_ID();
//		Ped myPed = PLAYER::GET_PLAYER_PED(ped);
//		//Vector3 position = GET_COORDS_INFRONT(5).add(ENTITY::GET_ENTITY_COORDS(ped, true, false)).add({ 0,0,400 });
//		position = GET_COORDS_INFRONT(5).add(ENTITY::GET_ENTITY_COORDS(ped, true, false)).add({ 0,0,400 });
//		Hash model = 0xB72F3DA7;
//		//Hash modelHash = GAMEPLAY::GET_HASH_KEY((char*)0xB72F3DA7);				
//		if (STREAMING::IS_MODEL_VALID(model) && STREAMING::IS_MODEL_IN_CDIMAGE(model))
//		{
//			//while (!STREAMING::HAS_MODEL_LOADED(model))	//加载模型
//			//{
//			//	WaitAndDraw(0); // 等待(0);
//			//}
//			Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 3.0, -0.3);	//获取玩家相对坐标
//			Vector3 myPos = coords;
//
//			uint32_t createdPed = OBJECT::CREATE_OBJECT(model, myPos.x, myPos.y, myPos.z, 1, 1, 0, 0, 1);	//创建物体
//			ENTITY::SET_ENTITY_VISIBLE(createdPed, true);	//设置物体可见
//			ENTITY::SET_ENTITY_ALPHA(createdPed, 255, false);	//设置透明度
//			PED::SET_PED_VISIBLE(createdPed, true);	// 设置PED为可见
//			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(createdPed, false, false);//make it despawn 设置为任务实体
//			int createdPedCopy = createdPed;
//			ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&createdPedCopy);	//设置为不再需要
//			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);	// 设置模型为不再需要
//
//			ufoID = createdPed;
//			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(createdPed, true, true);	//设置为任务实体
//			int blip = RADAR::_0x23F74C2FDA6E7C61(-515518185, createdPed);	//添加斑点
//			RADAR::SET_BLIP_NAME_FROM_TEXT_FILE(blip, "BLIP_DEBUG");	//一个有趣的东西
//			// 其中一个会在首次创建时让图标闪烁
//			// 另一个可使其在地图上不出现列表
//			// 不知道另一个有什么用
//			RADAR::_0x662D364ABF16DE2F(blip, 580546400);	//首次创建时让图标闪烁
//			RADAR::_0x662D364ABF16DE2F(blip, -1878373110);	//使其不在菜单列表中?可能与名称有关
//			RADAR::_0x662D364ABF16DE2F(blip, 231194138);
//
//			//CAM::_ANIMATE_GAMEPLAY_CAM_ZOOM(1.0f, 1.0f);	//游戏镜头放大
//
//			enabled = true;
//			SetStatusText(GT("完成"));
//		}
//		else
//		{
//			SetStatusText(GT("模型错误"));
//		}
//	}
//
//
//public:
//	MenuItemTest(string caption):
//		MenuItemDefault(caption) {}
//};

// 检测更新
class MenuItemTest : public MenuItemDefault
{

	virtual void OnSelect()
	{
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), _T("https://mod.3dmgame.com/mod/170521"), NULL, SW_SHOW);
	}

public:
	MenuItemTest(string caption)
		: MenuItemDefault(caption) {}
};


MenuBase* CreateMainMenu(MenuController* controller)
{
	auto v = CheckUpdates();
	auto menu = new MenuBase(new MenuItemTitle(GT("天基轨道炮")));
	controller->RegisterMenu(menu);

	menu->AddItem(new SpawnDriveableUFO(GT("启动天基轨道炮")));

	menu->AddItem(new MenuItemRangeSweep(GT("范围扫荡 [慎用!]")));

	menu->AddItem(new MenuItemTest(GT(v)));

	return menu;
}