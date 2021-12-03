#pragma once
#include <Windows.h>
#include <vector>
#include <d2d1.h>
#include <wincodec.h>

/*

DrawItem struct

used to store information about images that have been loaded by the
screen class

*/

struct DrawItem
{
	int iWidth;
	int iHeight;

	float fDrawTime;
	float fTransitionTime;

	ID2D1Bitmap* p_Picture = 0;
};

/*

Screen class

designed to help with the loading, storing, and drawing of images
with DirectX

created by: Joshua Print

*/

class Screen
{
public:
	Screen();
	~Screen();

	bool Startup(HWND);

	static void SetScreenSize(int, int);

	bool AddSprite(DrawItem*, const char*);

	void Update();
	void Draw();
private:
	static int iScreenWidth;
	static int iScreenHeight;

	float fDelta = 0.0f;
	float fRollingDelta = 0.0f;
	
	float fAlphaA;
	float fAlphaB;

	bool bChangingSlide = false;

	int iCurrentSlide = 0;
	int iMaxSlide = 0;

	int iFirstFood = 0;
	int iSecondFood = 1;
	int iThirdFood = 2;
	int iMaxFood = 0;

	HWND WindowsHWND;
	ID2D1Factory* p_DirectXFactory = 0;
	ID2D1HwndRenderTarget* p_RenderTarget = 0;

	IWICImagingFactory* pIWCFactory = 0;

	std::vector<DrawItem*> p_vPictures;
	std::vector<DrawItem*> p_vMenu;
};