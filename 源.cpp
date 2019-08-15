#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "BaseWindow.h"
#include <d2d1.h>
#include <windowsx.h>
#include <dwrite.h>
#include "powerscheme.h"

template<class T> void SafeRelease(T** ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = NULL;
	}
}

BOOL inRect(LPARAM pos, D2D1_RECT_F rect) {
	int x = GET_X_LPARAM(pos);
	int y = GET_Y_LPARAM(pos);

	if (x<rect.right && x>rect.left && y>rect.top && y<rect.bottom)
		return TRUE;
	else
		return FALSE;
}

const D2D1_COLOR_F Red = D2D1::ColorF(0xF65314);
const D2D1_COLOR_F Green = D2D1::ColorF(0x7CBB00);
const D2D1_COLOR_F Blue= D2D1::ColorF(0x00A1F1);
const D2D1_COLOR_F Yellow = D2D1::ColorF(0xFFBB00);
const D2D1_COLOR_F DarkCover = D2D1::ColorF(D2D1::ColorF::Black, 0.6f);
const D2D1_COLOR_F LightCover = D2D1::ColorF(D2D1::ColorF::White, 0.4f);

const float sideBarWidth = 50.f;

class MainWindow : public BaseWindow<MainWindow> {
	PowerScheme powerScheme;

	ID2D1Factory* pFactory = NULL;
	ID2D1HwndRenderTarget* pRenderTarget = NULL;
	ID2D1SolidColorBrush* pBrushRed = NULL;
	ID2D1SolidColorBrush* pBrushGreen = NULL;
	ID2D1SolidColorBrush* pBrushBlue = NULL;
	ID2D1SolidColorBrush* pBrushYellow = NULL;
	ID2D1SolidColorBrush* pBrushWhite = NULL;
	ID2D1SolidColorBrush* pBrushBlack= NULL;
	ID2D1SolidColorBrush* pBrushDrakCover = NULL;
	ID2D1SolidColorBrush* pBrushLightCover = NULL;

	IDWriteFactory* pDWriteFactory = NULL;
	IDWriteTextFormat* pTextFormatYaHei = NULL;
	IDWriteTextFormat* pTextFormatSegoe= NULL;
	const wchar_t* wszTextTl = NULL;
	const wchar_t* wszTextTr = NULL;
	const wchar_t* wszTextBl = NULL;
	const wchar_t* wszTextBr = NULL;
	const wchar_t* wszTextHome = NULL;
	
	D2D1_RECT_F rectTl;
	D2D1_RECT_F rectTr;
	D2D1_RECT_F rectBl;
	D2D1_RECT_F rectBr;
	D2D1_RECT_F rectAll;
	D2D1_RECT_F rectSideBar;
	D2D1_RECT_F rectHome;

	float x = 0.f, y = 0.f; //整个Client

	HCURSOR arrowCursor = LoadCursor(NULL, IDC_ARROW);
	HCURSOR waitCursor = LoadCursor(NULL, IDC_WAIT);

	LRESULT CreateGraphicsResources();
	void DiscardGraphcisResources();
	void CalculateLayout();
	void OnPaint();
	void ReSize();
	void SchemeSelect(LPARAM lParam);
	void FillSchemeRect(int rect = 0);

public:
	PCWSTR ClassName() const { return L"Mode Changer Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

LRESULT MainWindow::CreateGraphicsResources() {
	HRESULT hr = S_OK;

	if (pRenderTarget == NULL) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget
		);

		pRenderTarget->CreateSolidColorBrush(Red, &pBrushRed);
		pRenderTarget->CreateSolidColorBrush(Green, &pBrushGreen);
		pRenderTarget->CreateSolidColorBrush(Blue, &pBrushBlue);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrushWhite);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBrushBlack);
		pRenderTarget->CreateSolidColorBrush(DarkCover, &pBrushDrakCover);
		pRenderTarget->CreateSolidColorBrush(LightCover, &pBrushLightCover);
		hr = pRenderTarget->CreateSolidColorBrush(Yellow, &pBrushYellow);

		if (SUCCEEDED(hr)) {
			wszTextTl = L"平衡";
			wszTextTr = L"DELL";
			wszTextBl = L"下载";
			wszTextBr = L"Gaming";
			wszTextHome = L"";

			pDWriteFactory->CreateTextFormat(
				L"Segoe MDL2 Assets",
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				36.0f,
				L"zh-cn",
				&pTextFormatSegoe
			);

			pDWriteFactory->CreateTextFormat(
				L"Microsoft YaHei UI",
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				72.0f,
				L"zh-cn",
				&pTextFormatYaHei
			);

			pTextFormatSegoe->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pTextFormatSegoe->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

			pTextFormatYaHei->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			hr = pTextFormatYaHei->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		if (SUCCEEDED(hr)) {
			CalculateLayout();
		}
	}

	return hr;
}

void MainWindow::DiscardGraphcisResources() {
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrushRed);
	SafeRelease(&pBrushGreen);
	SafeRelease(&pBrushBlue);
	SafeRelease(&pBrushYellow);
	SafeRelease(&pBrushWhite);
	SafeRelease(&pBrushBlack);
	SafeRelease(&pBrushDrakCover);
	SafeRelease(&pBrushLightCover);
	SafeRelease(&pTextFormatYaHei);
	SafeRelease(&pTextFormatSegoe);
	SafeRelease(&pFactory);
	SafeRelease(&pDWriteFactory);
}

void MainWindow::CalculateLayout() {
	if (pRenderTarget != NULL) {
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		x = size.width;
		y = size.height;
		rectTl = D2D1::RectF(sideBarWidth, 0.f, (x + sideBarWidth) / 2, y / 2);
		rectTr = D2D1::RectF((x + sideBarWidth) / 2, 0.f, x, y / 2);
		rectBl = D2D1::RectF(sideBarWidth, y / 2, (x + sideBarWidth) / 2, y);
		rectBr = D2D1::RectF((x + sideBarWidth) / 2, y / 2, x, y);
		rectAll = D2D1::RectF(sideBarWidth, 0.f, x, y);
		rectSideBar = D2D1::RectF(0, 0, 50, y * 2);
		rectHome = D2D1::RectF(0, 0, 50, 50);
	}
}

void MainWindow::OnPaint() {
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr)) {
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear();
		pRenderTarget->FillRectangle(&rectSideBar, pBrushBlack);

		pRenderTarget->DrawTextW(wszTextHome, (UINT32)wcslen(wszTextHome), pTextFormatSegoe, rectHome, pBrushWhite);
		
		pRenderTarget->FillRectangle(&rectHome, pBrushLightCover);

		FillSchemeRect();

		pRenderTarget->FillRectangle(&rectAll, pBrushDrakCover);

		FillSchemeRect(powerScheme.getActiveCfg());
		
		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			DiscardGraphcisResources();
		}

		EndPaint(m_hwnd, &ps);
	}
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
			return -1;
		}

		if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown * *>(&pDWriteFactory)))) {
			return -2;
		}

		return 0;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}

	case WM_PAINT: {
		OnPaint();
		return 0;
	}

	/*
	case WM_CLOSE: {
		if (MessageBox(m_hwnd, L"确定要关闭吗？", L"Mode Changer", MB_OKCANCEL) == IDOK)
			DestroyWindow(m_hwnd);
		return 0;
	}
	*/

	case WM_SIZE: {
		ReSize();
		return 0;
	}

	case WM_LBUTTONUP: {
		SetCursor(waitCursor);

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		if (x > 50)
			SchemeSelect(lParam);

		OnPaint();

		SetCursor(arrowCursor);

		return 0;
	}

	case WM_SETCURSOR: {
		SetCursor(arrowCursor);
	}
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);

}

void MainWindow::ReSize() {
	if (pRenderTarget != NULL) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void MainWindow::SchemeSelect(LPARAM lParam) {
	if (inRect(lParam, rectTl))
		powerScheme.changecfg(1);
	else if (inRect(lParam, rectTr))
		powerScheme.changecfg(2);
	else if (inRect(lParam, rectBl))
		powerScheme.changecfg(3);
	else if (inRect(lParam, rectBr))
		powerScheme.changecfg(4);
}

void MainWindow::FillSchemeRect(int rect) {
	switch (rect) {
	case 0:
		FillSchemeRect(1);
		FillSchemeRect(2);
		FillSchemeRect(3);
		FillSchemeRect(4);
		break;
	case 1:
		pRenderTarget->FillRectangle(&rectTl, pBrushRed);
		pRenderTarget->DrawTextW(wszTextTl, (UINT32)wcslen(wszTextTl), pTextFormatYaHei, rectTl, pBrushWhite);
		break;
	case 2:
		pRenderTarget->FillRectangle(&rectTr, pBrushGreen);
		pRenderTarget->DrawTextW(wszTextTr, (UINT32)wcslen(wszTextTr), pTextFormatYaHei, rectTr, pBrushWhite);
		break;
	case 3:
		pRenderTarget->FillRectangle(&rectBl, pBrushBlue);
		pRenderTarget->DrawTextW(wszTextBl, (UINT32)wcslen(wszTextBl), pTextFormatYaHei, rectBl, pBrushWhite);
		break;
	case 4:
		pRenderTarget->FillRectangle(&rectBr, pBrushYellow);
		pRenderTarget->DrawTextW(wszTextBr, (UINT32)wcslen(wszTextBr), pTextFormatYaHei, rectBr, pBrushWhite);
		break;
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	MainWindow win;

	if (!win.Create(L"Mode Changer", WS_OVERLAPPEDWINDOW)) {
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	/*---消息循环---*/
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}