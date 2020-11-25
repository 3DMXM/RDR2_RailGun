#include "RailGun.h"
#include <atlbase.h>



// ȫ�ֱ��� ��
const double PI = 3.141592653589793238463;
// ������������
class SpawnDriveableUFO : public MenuItemSwitchable
{

	bool missileActive;
	Vector3 location = { 0,0,0 };
	Vector3 rotation = { 0,0,0 };
	Vector3 velocity = { 0,0,0 };
	float dropSpeed = 0.0f;	//��׹�ٶ�
	float Speed = 1.0f;	//�ƶ��ٶ�
	float initialZForGroundTest = 0;

	virtual void OnSelect()
	{
		//�������½���ʾ��ť
		static int LensMoveUD = -1;
		static int LensMoveLR = -1;
		static int LensMoveEnlarge = -1;
		static int LensMoveShrink = -1;
		static int LensMoveFire = -1;

		createPrompt(&LensMoveUD, INPUT_MOVE_UD, GT("�����ƶ�"));
		createPrompt(&LensMoveLR, INPUT_MOVE_LR, GT("�����ƶ�"));
		createPrompt(&LensMoveEnlarge, Key_Shift, GT("��ͷ�Ŵ�"));
		createPrompt(&LensMoveShrink, INPUT_DUCK, GT("��ͷ��С"));
		createPrompt(&LensMoveFire, INPUT_JUMP, GT("����"));

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

			CONTROLS::DISABLE_CONTROL_ACTION(1, INPUT_DUCK, false);	//������ʱ������Ҳٿؽ�ɫ
		}
		SetState(!GetState());
	}

	virtual void OnFrame()
	{
		if (GetState())
		{
			// ���Ƶ���UI
			DrawGameRect(0.5f, 0.5f, 1.0f, 1.0f, 56, 173, 169, 150);//���Ʊ��� rgb(47, 53, 66)
			drawBoxOutline(0.5f, 0.5f, 0.001f, 0.005f, 255, 255, 255, 255);	// �������ĵ�
			drawBoxOutline(0.5f, 0.5f, 0.05f, 0.005f, 255, 255, 255, 255);	//�������ķ���
			drawBoxOutline(0.5f, 0.5f, 0.65f, 0.3f, 0.005f, 255, 255, 255, 255);	//���ƴ󷽿�
			drawRedboxes();	//��ʾNPC��λ��
			drawGreenBoxesForPlayer(); //��ʾ��ҵ�λ��

			Vector3 velocityFinal = GET_COORDS_INFRONT_OF_ANGLE(rotation, dropSpeed);
			/*
			  x�� + : ���ƶ�
			  x�� - : ���ƶ�
			  z�� + : ���ƶ�
			  z�� - : ���ƶ�
			  y�� + : �Ŵ�
			  y�� - : ��С
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

			if (location.z <= onZ + 30) location.z = onZ + 30;	// �ж��Ƿ��Ѵ���������

			if (IsKeyDown(32)) //space
			{
				// ����....
				//Vector3 coords = GetCoordsInfrontOfCam(0.1f);
				Vector3 coords = location;
				coords.z = onZ;
				FIRE::ADD_OWNED_EXPLOSION(PLAYER::PLAYER_PED_ID(), coords.x, coords.y, coords.z, 11, 1000, true, false, 10);
				//SetStatusText(GT("����" + to_string(location.z) + "." + to_string(onZ)));
				//SetStatusText(GT("��ը���꣺" + to_string(coords.x) + to_string(coords.y) + to_string(coords.z)));
			}

			//GRAPHICS::DRAW_LIGHT_WITH_RANGE(location.x, location.y, onZ, 255, 0, 0, 1000, 10000);	//���Ƽ���


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
			GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, initialZForGroundTest, &groundZ, true);//true ʹ��ͷ��׼����
			camShit::loop();
		}
	}

public:
	SpawnDriveableUFO(string caption)
		: MenuItemSwitchable(caption) {}
};
// ��Χɨ��
class MenuItemRangeSweep : public MenuItemSwitchable
{	
	virtual void OnFrame()
	{
		if (GetState())
		{
			drawBoxOutline(0.5f, 0.5f, 0.001f, 0.005f, 255, 255, 255, 255);	// �������ĵ�
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
//// ���԰�ť
//class MenuItemTest :public MenuItemDefault
//{
//	bool enabled = false;
//	Vector3 position = { 0,0,0 };
//	Vector3 velocity = { 0,0,0 };
//	float minHeight = 0.0f;
//	float maxHeightToLane = 20.0f;
//	float slowModeHeight = 40.0f;
//
//	// ��ȡ����Z��
//	float getHeightAboveGround() {
//		float groundZ = 0;
//		GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(position.x, position.y, 1000.0f, &groundZ, true);
//		return position.z - groundZ;
//	}
//
//	virtual void OnSelect()
//	{
//		//SetStatusText(GT("����"));
//		//�������½���ʾ��ť
//		//int promptShift = -1;
//		//int promptF = -1;
//		//bool controllerMode = false;
//		//createPrompt(&promptShift, controllerMode ? Buttons::Button_L1 : ButtonHashKey::Key_Shift, GT("���Shift"));
//		//createPrompt(&promptF, controllerMode ? Buttons::Button_Triangle : ButtonHashKey::Key_F, GT("����UFO"));//Changed to F    //enter  1138488863 0x43DBF61F
//		//setPromptEnabled(&promptShift, true);
//		//setPromptEnabled(&promptF, true);
//
//		//����UFO
//		int ufoID = -1;
//		Ped ped = PLAYER::PLAYER_PED_ID();
//		Ped myPed = PLAYER::GET_PLAYER_PED(ped);
//		//Vector3 position = GET_COORDS_INFRONT(5).add(ENTITY::GET_ENTITY_COORDS(ped, true, false)).add({ 0,0,400 });
//		position = GET_COORDS_INFRONT(5).add(ENTITY::GET_ENTITY_COORDS(ped, true, false)).add({ 0,0,400 });
//		Hash model = 0xB72F3DA7;
//		//Hash modelHash = GAMEPLAY::GET_HASH_KEY((char*)0xB72F3DA7);				
//		if (STREAMING::IS_MODEL_VALID(model) && STREAMING::IS_MODEL_IN_CDIMAGE(model))
//		{
//			//while (!STREAMING::HAS_MODEL_LOADED(model))	//����ģ��
//			//{
//			//	WaitAndDraw(0); // �ȴ�(0);
//			//}
//			Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 3.0, -0.3);	//��ȡ����������
//			Vector3 myPos = coords;
//
//			uint32_t createdPed = OBJECT::CREATE_OBJECT(model, myPos.x, myPos.y, myPos.z, 1, 1, 0, 0, 1);	//��������
//			ENTITY::SET_ENTITY_VISIBLE(createdPed, true);	//��������ɼ�
//			ENTITY::SET_ENTITY_ALPHA(createdPed, 255, false);	//����͸����
//			PED::SET_PED_VISIBLE(createdPed, true);	// ����PEDΪ�ɼ�
//			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(createdPed, false, false);//make it despawn ����Ϊ����ʵ��
//			int createdPedCopy = createdPed;
//			ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&createdPedCopy);	//����Ϊ������Ҫ
//			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);	// ����ģ��Ϊ������Ҫ
//
//			ufoID = createdPed;
//			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(createdPed, true, true);	//����Ϊ����ʵ��
//			int blip = RADAR::_0x23F74C2FDA6E7C61(-515518185, createdPed);	//��Ӱߵ�
//			RADAR::SET_BLIP_NAME_FROM_TEXT_FILE(blip, "BLIP_DEBUG");	//һ����Ȥ�Ķ���
//			// ����һ�������״δ���ʱ��ͼ����˸
//			// ��һ����ʹ���ڵ�ͼ�ϲ������б�
//			// ��֪����һ����ʲô��
//			RADAR::_0x662D364ABF16DE2F(blip, 580546400);	//�״δ���ʱ��ͼ����˸
//			RADAR::_0x662D364ABF16DE2F(blip, -1878373110);	//ʹ�䲻�ڲ˵��б���?�����������й�
//			RADAR::_0x662D364ABF16DE2F(blip, 231194138);
//
//			//CAM::_ANIMATE_GAMEPLAY_CAM_ZOOM(1.0f, 1.0f);	//��Ϸ��ͷ�Ŵ�
//
//			enabled = true;
//			SetStatusText(GT("���"));
//		}
//		else
//		{
//			SetStatusText(GT("ģ�ʹ���"));
//		}
//	}
//
//
//public:
//	MenuItemTest(string caption):
//		MenuItemDefault(caption) {}
//};

// ������
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
	auto menu = new MenuBase(new MenuItemTitle(GT("��������")));
	controller->RegisterMenu(menu);

	menu->AddItem(new SpawnDriveableUFO(GT("������������")));

	menu->AddItem(new MenuItemRangeSweep(GT("��Χɨ�� [����!]")));

	menu->AddItem(new MenuItemTest(GT(v)));

	return menu;
}