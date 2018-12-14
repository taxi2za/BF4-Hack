#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

IDirect3D9Ex* dx_Object = NULL;
IDirect3DDevice9Ex* dx_Device = NULL;
D3DPRESENT_PARAMETERS dx_Param;
char txt[1024];
ID3DXFont* dx_Font = 0;

int D3DIniti(HWND hWnd)
{
	if(FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &dx_Object)))
		exit(1);

	dx_Param.BackBufferFormat	= D3DFMT_A8R8G8B8;
	dx_Param.hDeviceWindow		= hWnd;
	dx_Param.MultiSampleQuality = DEFAULT_QUALITY;
	dx_Param.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	dx_Param.Windowed			= true;

	if(FAILED(dx_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dx_Param, 0, &dx_Device)))
		exit(1);

	D3DXCreateFont(dx_Device, 15, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Verdana", &dx_Font);

	return 0;
}
#pragma region DirectX
void DrawFilledBox(int x, int y, int w, int h, int r, int g, int b, int a) 
{ 
	static ID3DXLine* pLine;

    if (!pLine)
		D3DXCreateLine(dx_Device, &pLine); 

    D3DXVECTOR2 vLine[2]; 

    pLine->SetWidth( w ); 
    pLine->SetAntialias( false ); 
    pLine->SetGLLines( true ); 
     
    vLine[0].x = x + w/2; 
    vLine[0].y = y; 
    vLine[1].x = x + w/2; 
    vLine[1].y = y + h; 

    pLine->Begin( ); 
    pLine->Draw( vLine, 2, D3DCOLOR_RGBA( r, g, b, a ) ); 
    pLine->End( ); 
} 

void DrawBox(int x, int y, int width, int height, int px, int r, int g, int b, int a) 
{ 
    DrawFilledBox(x,y+height,width,px,r,g,b,a); 
    DrawFilledBox(x-px,y,px,height,r,g,b,a); 
    DrawFilledBox(x,y-px,width,px,r,g,b,a); 
    DrawFilledBox(x+width,y,px,height,r,g,b,a); 
} 

int DrawText(char* String, int x, int y, int r, int g, int b)
{
	RECT Font;
	Font.bottom = 0;
	Font.left = x;
	Font.top = y;
	Font.right = 0;

	dx_Font->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));

	return 0;
}

int DrawShadowText(char* String, int x, int y, int r, int g, int b)
{
	RECT Font;
	Font.bottom = 0;
	Font.left = x;
	Font.top = y;
	Font.right = 0;

	RECT Fonts;
	Fonts.bottom = 0;
	Fonts.left = x+1;
	Fonts.top = y+1;
	Fonts.right = 0;

	dx_Font->DrawTextA(0, String, strlen(String), &Fonts, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	dx_Font->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));

	return 0;
}
struct D3DTLVERTEX
{
	float X, Y, Z, RHW;
	DWORD Color;
};

void DrawLine(float startX, float startY, float endX, float endY, D3DCOLOR dwColour, LPDIRECT3DDEVICE9 pDev)
{
	D3DTLVERTEX Vertex[2] = {
		{ (float)startX, (float)startY, 0.0f, 1.0f, dwColour },
		{ (float)endX, (float)endY, 1.0f, 1.0f, dwColour },
	};

	const DWORD FVFTL = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	IDirect3DBaseTexture9* pTexture = NULL;

	DWORD dwLighting = 0, dwZEnable = 0, dwZFunc = 0, dwFVF = 0, dwColorOp = 0, dwColorArg = 0;

	dx_Device->GetTexture(0, &pTexture);
	dx_Device->GetRenderState(D3DRS_LIGHTING, &dwLighting);
	dx_Device->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	dx_Device->GetRenderState(D3DRS_ZFUNC, &dwZFunc);
	dx_Device->GetFVF(&dwFVF);
	dx_Device->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
	dx_Device->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg);

	dx_Device->SetTexture(0, NULL);
	dx_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	dx_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	dx_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	dx_Device->SetFVF(FVFTL);
	dx_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	dx_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	dx_Device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &Vertex, sizeof(D3DTLVERTEX));

	dx_Device->SetTexture(0, pTexture);
	dx_Device->SetRenderState(D3DRS_LIGHTING, dwLighting);
	dx_Device->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	dx_Device->SetRenderState(D3DRS_ZFUNC, dwZFunc);
	dx_Device->SetFVF(dwFVF);
	dx_Device->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
	dx_Device->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg);
}

void DrawOutline(float x, float y, float w, float h, D3DCOLOR dwColour, LPDIRECT3DDEVICE9 pDev)
{
	D3DTLVERTEX Vertex[5] = {
		{ (float)x, (float)y, 0.0f, 1.0f, dwColour },
		{ (float)(x + w), (float)y, 0.0f, 1.0f, dwColour },
		{ (float)(x + w), (float)(y + h), 0.0f, 1.0f, dwColour },
		{ (float)x, (float)(y + h), 0.0f, 1.0f, dwColour },
		{ (float)x, (float)y, 0.0f, 1.0f, dwColour }
	};

	const DWORD FVFTL = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	IDirect3DBaseTexture9* pTexture = NULL;

	DWORD dwLighting = 0, dwZEnable = 0, dwZFunc = 0, dwFVF = 0, dwColorOp = 0, dwColorArg = 0;

	dx_Device->GetTexture(0, &pTexture);
	dx_Device->GetRenderState(D3DRS_LIGHTING, &dwLighting);
	dx_Device->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	dx_Device->GetRenderState(D3DRS_ZFUNC, &dwZFunc);
	dx_Device->GetFVF(&dwFVF);
	dx_Device->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
	dx_Device->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg);

	dx_Device->SetTexture(0, NULL);
	dx_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	dx_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	dx_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	dx_Device->SetFVF(FVFTL);
	dx_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	dx_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	dx_Device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &Vertex, sizeof(D3DTLVERTEX));

	dx_Device->SetTexture(0, pTexture);
	dx_Device->SetRenderState(D3DRS_LIGHTING, dwLighting);
	dx_Device->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	dx_Device->SetRenderState(D3DRS_ZFUNC, dwZFunc);
	dx_Device->SetFVF(dwFVF);
	dx_Device->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
	dx_Device->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg);
}
#pragma endregion

void Draw();
int RenderIt()
{
	dx_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	dx_Device->BeginScene();

	
	DrawBox(189, 96, 181, 21, 1, 200, 200, 200, 200);
	DrawFilledBox(190, 97, 180, 20, 100, 100, 100, 200);
	DrawShadowText("Visit UnknownCheats.me!", 200, 100, 255, 255, 255);
	

	Draw();

	dx_Device->EndScene();
	dx_Device->PresentEx( 0, 0, 0, 0, 0);
	return 0;
}


//==============================================================================================================================
// OFFSET
//==============================================================================================================================
#define ADDRESS_CLIENTGAMECONTEXT    0x142670d80
#define ADDRESS_DXRENDERER           0x142738080 
#define ADDRESS_GAMERENDERER         0x142672378
#define ADDRESS_ANGLES				 0x1423b2ec0 
#define ADDRESS_WORLDRENDERSETTINGS  0x1426724a0 
//===============================================================
#define PlayerManager			0x60
#define lPLAYERARRAY			0x540
#define pPLAYERARRAY			0x548
#define TeamId		            0x13CC
#define pSOLDIER		        0x14D0
#define Occluded		        0x5B1
#define Red						D3DCOLOR_ARGB(255,204,46,46)
#define Orange					D3DCOLOR_ARGB(255,255,255,0)
#define Green					D3DCOLOR_ARGB(255,34,161,52)
#define Blue					D3DCOLOR_ARGB(255,45,105,196)
//===============================================================

extern Memory memory;

float getDistance(D3DXVECTOR3  me, D3DXVECTOR3 you){

	float tempx, tempy, tempz, tempA;

	tempx = (me.x - you.x)*(me.x - you.x);
	tempy = (me.y - you.y)*(me.y - you.y);
	tempz = (me.z - you.z)*(me.z - you.z);

	tempA = tempx + tempy + tempz;

	if(tempA < 0)
		tempA = (tempA * (-1));

	return sqrt(tempA);
}
DWORD_PTR pClientGameContext, GameRenderer,DxRenderer, pPlayerManager, local_player, pPlayers, ClientPlayer;
D3DXMATRIXA16 ViewProjection;

bool WorldToScreen(const D3DXVECTOR3& world_point, D3DXVECTOR3& out)
{
	GameRenderer = memory.read<DWORD_PTR>(ADDRESS_GAMERENDERER);
	if (!memory.IsValid(GameRenderer))
		return false;

	DWORD_PTR pRenderView = memory.read<DWORD_PTR>(GameRenderer + 0x60);
	if (!memory.IsValid(pRenderView))
		return false;
	
	auto view_x_projection = memory.read<D3DXMATRIXA16>(pRenderView + 0x420);

	DxRenderer = memory.read<DWORD_PTR>(ADDRESS_DXRENDERER);
	if (!memory.IsValid(DxRenderer))
		return false;
	
	DWORD_PTR m_pScreen = memory.read<DWORD_PTR>(DxRenderer + 0x38);
	if (!memory.IsValid(m_pScreen))
		return false;
	
	int ScreenWidth, ScreenHeight;
	 ScreenWidth = memory.read<int>(m_pScreen + 0x58);
	 ScreenHeight = memory.read<int>(m_pScreen + 0x5C);


	float cX = ScreenWidth * 0.5f;
	float cY = ScreenHeight * 0.5f;

	auto x = view_x_projection(0, 0) * world_point.x + view_x_projection(1, 0) * world_point.y + view_x_projection(2, 0) * world_point.z + view_x_projection(3, 0);
	auto y = view_x_projection(0, 1) * world_point.x + view_x_projection(1, 1) * world_point.y + view_x_projection(2, 1) * world_point.z + view_x_projection(3, 1);
	auto z = view_x_projection(0, 2) * world_point.x + view_x_projection(1, 2) * world_point.y + view_x_projection(2, 2) * world_point.z + view_x_projection(3, 2);
	auto w = view_x_projection(0, 3) * world_point.x + view_x_projection(1, 3) * world_point.y + view_x_projection(2, 3) * world_point.z +view_x_projection(3, 3);

	if (w < 0.65f)
	{
		out.z = w;
		return false;
	}

	out.x = cX * (1 + x / w);
	out.y = cY * (1 - y / w);
	out.z = w;

	return true;
}

D3DXVECTOR3 GetBone(INT64 pSoldier, int bone_id)
{
	D3DXVECTOR3 tmp, out;
	DWORD_PTR ragdoll_component = memory.read<DWORD_PTR>(pSoldier + 0x580);
	if (!memory.IsValid(ragdoll_component))
		return D3DXVECTOR3(0, 0, 0);

	DWORD_PTR quat = memory.read<DWORD_PTR>(ragdoll_component + 0xB0);
	if (!memory.IsValid(quat))
		return D3DXVECTOR3(0, 0, 0);

	tmp = memory.read<D3DXVECTOR3>(quat + bone_id * 0x20);

	out.x = tmp.x;
	out.y = tmp.y;
	out.z = tmp.z;

	return out;
}

void Draw(){
	//=======================================================================
	//Local Player
	//=======================================================================
	//DrawShadowText("0", 190, 110, 255, 255, 255);
	D3DXVECTOR3 LocalPlayerPos;
	pClientGameContext = memory.read<DWORD_PTR>(ADDRESS_CLIENTGAMECONTEXT);
	if (!memory.IsValid(pClientGameContext))
		return;

	pPlayerManager = memory.read<DWORD_PTR>(pClientGameContext + PlayerManager);
	if (!memory.IsValid(pPlayerManager))
		return;
	
	local_player = memory.read<DWORD_PTR>(pPlayerManager + lPLAYERARRAY);
	if (!memory.IsValid(local_player))
		return;
	
	DWORD_PTR pPlayers = memory.read<DWORD_PTR>(pPlayerManager + pPLAYERARRAY);
	if (!memory.IsValid(pPlayers))
		return;

	for (unsigned int i = 0; i < 70; i++)
	{
		DWORD_PTR cur_player = memory.read<DWORD_PTR>(pPlayers + (i * 0x8));

		if (!memory.IsValid(cur_player))
			continue;

		if (cur_player == local_player)
			continue;

		DWORD_PTR local_TeamId = memory.read<DWORD_PTR>(local_player + TeamId);
		DWORD_PTR player_TeamId = memory.read<DWORD_PTR>(cur_player + TeamId);
		auto is_friendly = local_TeamId == player_TeamId;

		auto soldier = memory.read<DWORD_PTR>(cur_player + pSOLDIER);
		if (!memory.IsValid(soldier))
			continue;

		DWORD_PTR m_Occluded = memory.read<DWORD_PTR>(soldier + Occluded);
		auto is_visible = !m_Occluded;

		//char* Name = memory.ReadString(soldier + 0x1836, 100);
		float Health = memory.read<float>(memory.read<DWORD_PTR>(soldier + 0x140) + 0x20);
		D3DXVECTOR3 position_foot = memory.read<D3DXVECTOR3>(memory.read<INT64>(soldier + 0x490) + 0x30);
		D3DXVECTOR3 position_foot_screen;
		D3DXVECTOR3 BoneHead;

		sprintf(txt,"Health: %0.f",Health);
		WorldToScreen(position_foot, position_foot_screen);
		WorldToScreen(GetBone(soldier, 104), BoneHead);

		float BoxHeight = BoneHead.y - position_foot_screen.y;
		float BoxWidth = BoxHeight / 2.f;

			if (!is_visible)
			DrawOutline(BoneHead.x + 10, position_foot_screen.y, BoxWidth, BoxHeight, is_friendly ? Green  : Blue, dx_Device);
			else
			DrawOutline(BoneHead.x + 10, position_foot_screen.y, BoxWidth, BoxHeight, is_friendly ? Orange : Red, dx_Device);
		


		DrawShadowText(txt, position_foot_screen.x, position_foot_screen.y, 255, 255, 255);
		
#pragma region Skeleton
		int aSkeleton[][2] = 
		{
		{ 104, 142 },{ 142, 9 },{ 9, 11 },{ 11, 15 },
		{ 142,109},{109,111 },{111, 115},{ 142, 5 },
		{ 5,  188},{ 5, 197},{ 188, 184},{ 197, 198},
		};
		for (int i = 0; i < 12; ++i)
		{
			D3DXVECTOR3 Bone1 = GetBone(soldier, aSkeleton[i][0]);
			D3DXVECTOR3 Bone2 = GetBone(soldier, aSkeleton[i][1]);
			D3DXVECTOR3 Out1, Out2, Out3;
			if (WorldToScreen(Bone1, Out1) && WorldToScreen(Bone2, Out2))
			{
				DrawLine(Out1.x, Out1.y, Out2.x, Out2.y, is_friendly ? Orange : Blue, dx_Device);
			}
		}
#pragma endregion

	}

}