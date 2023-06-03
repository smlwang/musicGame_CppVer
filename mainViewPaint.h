#pragma once
#include <windows.h>
#include "mainView.h"
#include "mainViewInfo.h"
struct MainViewPainter {
	void PaintSongList(HDC hdc, MainView::SongList &songs) {
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, minfo.highlight.color);
		auto font = CreateFont(30, 19, 0, 0, 100, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
		SelectObject(hdc, font);
		int now = songs.idx;
		RECT mrect = minfo.song.midRect;
		DrawText(hdc, (">>  " + songs[now].name + "   <<").c_str(), -1, &mrect, DT_CENTER | DT_VCENTER);
		SetTextColor(hdc, minfo.default_color);
		for (int i = -1; i <= 3; i++) {
			if (now + i < 0 || i == 0 || i + now >= songs.size()) continue;
			RECT rect = mrect;
			rect.top += i * minfo.song.high;
			rect.bottom += i * minfo.song.high;
			DrawText(hdc, songs[now + i].name.c_str(), -1, &rect, DT_CENTER | DT_VCENTER);
		}
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);
	}
	void PaintBG(HDC hdc) {
		auto brush = CreateSolidBrush(RGB(0, 0, 0));
		SelectObject(hdc, brush);
		Rectangle(hdc, 0, 0, 900, 900);
		DeleteObject(brush);

		// LOGO
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, RGB(255, 255, 0));
		auto font = CreateFont(40, 20, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("JetBrains Mono"));
		SelectObject(hdc, font);
		RECT rect{ 400, 700, 800, 800 };
		DrawText(hdc, "Author: AsindE", -1, &rect, DT_CENTER | DT_VCENTER);
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);
	}
	void PaintTips(HDC hdc) {
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, minfo.default_color);
		auto font = CreateFont(28, 14, 0, 0, 100, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));
		SelectObject(hdc, font);
		RECT rect = minfo.helper.rect;
		DrawText(hdc, "Game Tips:\npress 'j' or 'k' to choose song\npress 's' to start game\n\nIn game:\npress 'p' to pause.\npress 'd f j k' to hit note.",
			-1, &rect, DT_CENTER | DT_VCENTER);
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);
	}
	void PaintMainView(HDC hdc, MainView &mw) {
		PaintBG(hdc);
		PaintTips(hdc);
		PaintSongList(hdc, mw.songList);
	}

	void PaintReplay(HDC hdc) {
		// TODO
	}

} mainViewPainter;