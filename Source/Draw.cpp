#include "Draw.h"
#include "Clock.h"

Screen::Screen()
{
}

Screen::~Screen()
{
}

//Function: AddSprite
//Paramaters: Pointer to a DrawItem struct for returning the stored picture
//            Character string for the filename to the picture to load
//Returns: Bool, True if no errors, false if something went wrong
//Description: Loads a picture from the passed filename using DirectX
//             and stores it in the DrawItem pointer
bool Screen::AddSprite(DrawItem* p_NewItem, const char* p_cFileName)
{
	bool bReturn = false;

	HRESULT hr;

	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pFrame = NULL;
	IWICFormatConverter* pConverter = NULL;

	// Make sure the class is ready to load the picture
	if (p_RenderTarget != 0 && pIWCFactory != 0 && p_NewItem != 0)
	{
		// Convert the ASCII character string to unicode
		int iCharacters = MultiByteToWideChar(CP_ACP, 0, p_cFileName, -1, NULL, 0);

		wchar_t* tempFilename = new wchar_t[iCharacters];

		MultiByteToWideChar(CP_ACP, 0, p_cFileName, -1, tempFilename, iCharacters);

		// load the corect decoder for the file format (uses the file extention)
		hr = pIWCFactory->CreateDecoderFromFilename(tempFilename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);

		if (SUCCEEDED(hr))
		{
			// Get the first frame
			hr = pDecoder->GetFrame(0, &pFrame);

			if (SUCCEEDED(hr))
			{
				// Create a format converter
				hr = pIWCFactory->CreateFormatConverter(&pConverter);

				if (SUCCEEDED(hr))
				{
					// Convert the format to the requested screen format
					hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);

					if (SUCCEEDED(hr))
					{						
						// Create the bitmap object with the data
						hr = p_RenderTarget->CreateBitmapFromWicBitmap(pConverter, &(p_NewItem->p_Picture));

						if (SUCCEEDED(hr))
						{
							//store the bitmap details and return to the caller
							D2D1_SIZE_F bitmapSize = p_NewItem->p_Picture->GetSize();
							
							p_NewItem->iHeight = bitmapSize.height;
							p_NewItem->iWidth = bitmapSize.width;

							p_NewItem->fDrawTime = 10.0f;
							p_NewItem->fTransitionTime = 0.5f;

							bReturn = true;
						}
					}

					pConverter->Release();
				}

				pDecoder->Release();
				pFrame->Release();
			}
		}
	}

	return bReturn;
}

//Function: Startup
//Paramaters: a windows HWND handle to tie directX to a window
//Returns: Bool, true if no errors, false if something went wrong
//Description: Performs all setup for the loading, drawing, and
//             processing of pictures
bool Screen::Startup(HWND hwnd)
{
	bool bReturn = false;

	HRESULT hr;

	// Create the directX factory (it makes directX drawing surfaces)
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p_DirectXFactory);

	if (hr == S_OK)
	{
		// Make a drawing surface in the windows windows has provided
		D2D1_SIZE_U ScreenSize;

		ScreenSize = D2D1::SizeU(iScreenWidth, iScreenHeight);

		hr = p_DirectXFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, ScreenSize), &p_RenderTarget);

		if (hr == S_OK)
		{
			// create the image factory (creates objects for maniuplating images)
			hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pIWCFactory);

			if (hr == S_OK)
			{
				// Search for pictures to load
				// These ones are for pictures that fade in and out
				HANDLE Search;
				WIN32_FIND_DATA FileData;
				char* cIsImage = 0;
				bool bImage = false;
				bool fFoundMore = false;

				bool bMenuReady = false;
				bool bFoodReady = false;

				Search = FindFirstFile("PicturesMenu\\*", &FileData);

				// Found valid pictures
				if (Search != INVALID_HANDLE_VALUE)
				{
					do
					{
						// Check the filename for image extentions
						cIsImage = strstr(FileData.cFileName, "gif");

						if (cIsImage != 0)
						{
							bImage = true;
						}


						cIsImage = strstr(FileData.cFileName, "jpg");

						if (cIsImage != 0)
						{
							bImage = true;
						}


						cIsImage = strstr(FileData.cFileName, "png");

						if (cIsImage != 0)
						{
							bImage = true;
						}

						cIsImage = strstr(FileData.cFileName, "bmp");

						if (cIsImage != 0)
						{
							bImage = true;
						}

						if (bImage)
						{
							// If an image has been found, try to load it
							DrawItem* pTemp = new DrawItem;

							if (pTemp != 0)
							{
								int iLength = strlen(FileData.cFileName) + strlen("PicturesMenu\\") + 1;

								char* cTemp = new char[iLength];

								strcpy_s(cTemp, iLength, "PicturesMenu\\");
								strcat_s(cTemp, iLength, FileData.cFileName);

								bool Created = AddSprite(pTemp, cTemp);

								if (Created)
								{
									// if its loaded, add it to the vector of loaded pictures
									pTemp->fDrawTime = 20.0f;

									p_vMenu.push_back(pTemp);

									iMaxSlide++;

									bMenuReady = true;
								}

								delete cTemp;
							}
						}

						fFoundMore = FindNextFile(Search, &FileData);

					} while (fFoundMore);

					// Make sure the vector has enough space for the pictures
					p_vMenu.reserve(sizeof(DrawItem) * iMaxSlide);

					// End the search
					FindClose(Search);
				}

				// Search for more pictures to load (diffrent location)
				// these pictures scroll across one half of the screen
				Search = FindFirstFile("PicturesFood\\*", &FileData);

				if (Search != INVALID_HANDLE_VALUE)
				{
					do
					{
						// Check file extentions...
						cIsImage = strstr(FileData.cFileName, "gif");

						if (cIsImage != 0)
						{
							bImage = true;
						}


						cIsImage = strstr(FileData.cFileName, "jpg");

						if (cIsImage != 0)
						{
							bImage = true;
						}


						cIsImage = strstr(FileData.cFileName, "png");

						if (cIsImage != 0)
						{
							bImage = true;
						}

						cIsImage = strstr(FileData.cFileName, "bmp");

						if (cIsImage != 0)
						{
							bImage = true;
						}

						if (bImage)
						{
							// Try to load...
							DrawItem* pTemp = new DrawItem;

							if (pTemp != 0)
							{
								int iLength = strlen(FileData.cFileName) + strlen("PicturesFood\\") + 1;

								char* cTemp = new char[iLength];

								strcpy_s(cTemp, iLength, "PicturesFood\\");
								strcat_s(cTemp, iLength, FileData.cFileName);

								bool Created = AddSprite(pTemp, cTemp);

								if (Created)
								{
									// Add to the vector...
									pTemp->fDrawTime = 5.0f;

									p_vPictures.push_back(pTemp);

									iMaxFood++;

									bFoodReady = true;
								}

								delete cTemp;
							}
						}

						fFoundMore = FindNextFile(Search, &FileData);

					} while (fFoundMore);

					// End the search
					FindClose(Search);

					p_vPictures.reserve(sizeof(DrawItem) * iMaxFood);

					// Return true if at least one of each picture was found
					bReturn = bFoodReady && bMenuReady;
				}
			}
		}
	}

	return bReturn;
}

//Function: SetScreenSize
//Paramaters: two Ints for the size of the screen, in pixels
//Returns: -None-
//Description: sets the size of the window, so it can be updated if needed
void Screen::SetScreenSize(int iWidth, int iHeight)
{
	if (iWidth > 0)
	{
		iScreenWidth = iWidth;
	}
	if (iHeight > 0)
	{
		iScreenHeight = iHeight;
	}
}

//Function: Update
//Paramaters: -None-
//Returns: -None-
//Description: updates the clock and sets what pictures are displayed
//             along with updating internal counters of time
void Screen::Update()
{
	// Update the clock to get time since last frame
	Clock::GetClock()->Update();

	float fNewDelta = Clock::GetClock()->GetDelta();

	iMaxSlide = p_vMenu.size();
	iMaxFood = p_vPictures.size();

	// Add the delta to the running total
	// the delta is used to make the animations smooth
	// no matter the computers clock speed
	fDelta = fDelta + fNewDelta;
	fRollingDelta = fRollingDelta + fNewDelta;

	// Check if the scrolling pictures need to change
	if (fRollingDelta >= p_vPictures.at(iFirstFood)->fDrawTime)
	{
		fRollingDelta = fRollingDelta - p_vPictures.at(iFirstFood)->fDrawTime;

		iFirstFood--;

		if (iFirstFood < 0)
		{
			iFirstFood = iMaxFood - 1;
		}

		iSecondFood--;

		if (iSecondFood < 0)
		{
			iSecondFood = iMaxFood - 1;
		}

		iThirdFood--;

		if (iThirdFood < 0)
		{
			iThirdFood = iMaxFood - 1;
		}
	}

	// check if the fading pictures have finished changing...
	if (bChangingSlide)
	{
		if (p_vMenu.at(iCurrentSlide)->fTransitionTime < fDelta)
		{
			fDelta = 0.0f;
			iCurrentSlide++;

			if (iCurrentSlide == iMaxSlide)
			{
				iCurrentSlide = 0;
			}

			bChangingSlide = false;
		}
	}
	// or if we need to start changing them
	else
	{
		if (p_vMenu.at(iCurrentSlide)->fDrawTime < fDelta && iMaxSlide > 1)
		{
			fDelta = 0.0f;

			bChangingSlide = true;
		}
	}
}

//Function: Draw
//Paramaters: -None-
//Returns: -None-
//Description: Draws loaded pictures to the screen using DirectX
void Screen::Draw()
{
	D2D1_RECT_F Source;
	D2D1_RECT_F Bitmap;

	D2D1_RECT_F FirstFoodBitmap;
	D2D1_RECT_F SecondFoodBitmap;
	D2D1_RECT_F ThirdFoodBitmap;

	D2D1_RECT_F FirstFoodSource;
	D2D1_RECT_F SecondFoodSource;
	D2D1_RECT_F ThirdFoodSource;

	// set where to draw each scrolling picture
	FirstFoodSource.left = iScreenWidth / 2;
	FirstFoodSource.right = iScreenWidth;
	FirstFoodSource.top = 0;
	FirstFoodSource.bottom = (fRollingDelta / p_vPictures.at(iFirstFood)->fDrawTime) * (iScreenHeight / 2);

	SecondFoodSource.left = iScreenWidth / 2;
	SecondFoodSource.right = iScreenWidth;
	SecondFoodSource.top = (fRollingDelta / p_vPictures.at(iFirstFood)->fDrawTime) * (iScreenHeight / 2);
	SecondFoodSource.bottom = ((fRollingDelta / p_vPictures.at(iFirstFood)->fDrawTime) * (iScreenHeight / 2)) + (iScreenHeight / 2);

	ThirdFoodSource.left = iScreenWidth / 2;
	ThirdFoodSource.right = iScreenWidth;
	ThirdFoodSource.top = ((fRollingDelta / p_vPictures.at(iFirstFood)->fDrawTime) * (iScreenHeight / 2)) + (iScreenHeight / 2);
	ThirdFoodSource.bottom = iScreenHeight;

	float fRatio;

	fRatio = FirstFoodSource.bottom / (iScreenHeight / 2);

	// set how much of each picture to draw
	FirstFoodBitmap.top = p_vPictures.at(iFirstFood)->iHeight - (p_vPictures.at(iFirstFood)->iHeight * fRatio);
	FirstFoodBitmap.left = 0;
	FirstFoodBitmap.right = p_vPictures.at(iFirstFood)->iWidth;
	FirstFoodBitmap.bottom = p_vPictures.at(iFirstFood)->iHeight;

	SecondFoodBitmap.top = 0;
	SecondFoodBitmap.left = 0;
	SecondFoodBitmap.right = p_vPictures.at(iSecondFood)->iWidth;
	SecondFoodBitmap.bottom = p_vPictures.at(iSecondFood)->iHeight;

	ThirdFoodBitmap.top = 0;
	ThirdFoodBitmap.left = 0;
	ThirdFoodBitmap.right = p_vPictures.at(iThirdFood)->iWidth;
	ThirdFoodBitmap.bottom = p_vPictures.at(iThirdFood)->iHeight * (1.0f - fRatio);

	// set where to draw the fading picture
	Source.left = 0;
	Source.top = 0;
	Source.bottom = iScreenHeight;
	Source.right = iScreenWidth / 2;

	// make sure we draw all of the fading picture
	Bitmap.top = 0;
	Bitmap.left = 0;
	Bitmap.right = p_vMenu.at(iCurrentSlide)->iWidth;
	Bitmap.bottom = p_vMenu.at(iCurrentSlide)->iHeight;

	// if the fading picture is changing
	if (bChangingSlide)
	{
		// set alpha values so we can blend pictures
		fAlphaB = fDelta / p_vMenu.at(iCurrentSlide)->fTransitionTime;
		fAlphaA = 1.0 - (fDelta / p_vMenu.at(iCurrentSlide)->fTransitionTime);

		int iNextSlide = iCurrentSlide + 1;

		if (iNextSlide == p_vMenu.size())
		{
			iNextSlide = 0;
		}

		// start drawing...
		p_RenderTarget->BeginDraw();

		// draw the image thats fading out
		p_RenderTarget->DrawBitmap(p_vMenu.at(iCurrentSlide)->p_Picture, &Source, fAlphaA, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &Bitmap);

		Bitmap.top = 0;
		Bitmap.left = 0;
		Bitmap.right = p_vMenu.at(iNextSlide)->iWidth;
		Bitmap.bottom = p_vMenu.at(iNextSlide)->iHeight;

		// draw the image thats fading in
		p_RenderTarget->DrawBitmap(p_vMenu.at(iNextSlide)->p_Picture, &Source, fAlphaB, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &Bitmap);
		
		// draw the scrolling pictures
		p_RenderTarget->DrawBitmap(p_vPictures.at(iFirstFood)->p_Picture, &FirstFoodSource, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &FirstFoodBitmap);
		p_RenderTarget->DrawBitmap(p_vPictures.at(iSecondFood)->p_Picture, &SecondFoodSource, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &SecondFoodBitmap);
		p_RenderTarget->DrawBitmap(p_vPictures.at(iThirdFood)->p_Picture, &ThirdFoodSource, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &ThirdFoodBitmap);

		// end drawing
		p_RenderTarget->EndDraw();
	}
	else
	{
		// start drawing
		p_RenderTarget->BeginDraw();

		// draw the (currently not) fading image
		p_RenderTarget->DrawBitmap(p_vMenu.at(iCurrentSlide)->p_Picture, &Source, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &Bitmap);

		// draw the scrolling images
		p_RenderTarget->DrawBitmap(p_vPictures.at(iFirstFood)->p_Picture, &FirstFoodSource, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &FirstFoodBitmap);
		p_RenderTarget->DrawBitmap(p_vPictures.at(iSecondFood)->p_Picture, &SecondFoodSource, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &SecondFoodBitmap);
		p_RenderTarget->DrawBitmap(p_vPictures.at(iThirdFood)->p_Picture, &ThirdFoodSource, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &ThirdFoodBitmap);

		// end drawing
		p_RenderTarget->EndDraw();
	}
}

int Screen::iScreenWidth = 0;
int Screen::iScreenHeight = 0;