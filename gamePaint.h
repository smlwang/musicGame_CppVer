#pragma once
#include "game.h"
struct GamePainter {
	char buffer[512];
	void PaintGame(HDC hdc, Game &game, long long now) {
		if (game.stat.code == game.stat.notReady) {
			return;
		}
		PaintBG(hdc, game);
		PaintNote(hdc, game, now);
		PaintInfo(hdc, game, now);
		PaintGameProccess(hdc, game, now);
		if (game.stat.code == game.stat.pause) {
			PaintPause(hdc);
		} else if (game.stat.code == game.stat.end) {
			PaintEnd(hdc, game);
		}
	}
	void PaintNote(HDC hdc, Game &game, long long now) {
		for (int i = 0; i < 4; i++) {
			auto &track = game.track[i].track;
			if (track.empty()) continue;
			auto st = track.rbegin();
			auto ed = track.rend();
			while (st != ed) {
				int left = Info.track.left(i);
				long perfect = Info.baseline.y - (st->st - now) * game.speed;
				long bottom = perfect + Info.note.adjust;

				if (bottom < 0) break;
				COLORREF color = Info.note.blue;
				if (i == 0 || i == 3) color = Info.note.white;
				if (st->kind == Info.note.click) {
					auto brush = CreateSolidBrush(color);
					SelectObject(hdc, brush);
					Rectangle(hdc, left, bottom - Info.note.high,
						left + Info.track.width, bottom);
					DeleteObject(brush);
					brush = CreateSolidBrush(Info.note.perfectJudge);
					SelectObject(hdc, brush);
					Rectangle(hdc, left, perfect - Info.note.perfectHigh,
						left + Info.track.width, perfect + Info.note.perfectHigh);
					DeleteObject(brush);
				} else {
					long top = Info.baseline.y - (st->ed - now) * game.speed;
					auto brush = CreateSolidBrush(color);
					SelectObject(hdc, brush);
					Rectangle(hdc, left + Info.note.adjust, top,
						left + Info.track.width - Info.note.adjust, bottom);
					if (!st->preCheck) {
						Rectangle(hdc, left, bottom - Info.note.bhigh,
							left + Info.track.width, bottom + Info.note.adjust);
					}
					DeleteObject(brush);
				}
				++st;
			}
		}
	}
	void PaintBG(HDC hdc, Game& game) {
		auto brush = CreateSolidBrush(RGB(0, 0, 0));
		SelectObject(hdc, brush);
		Rectangle(hdc, 0, 0, 900, 900);
		DeleteObject(brush);

		brush = CreateSolidBrush(Info.baseline.color);
		SelectObject(hdc, brush);
		Rectangle(hdc, Info.baseline.l, Info.baseline.y - 3,
			Info.baseline.r, Info.baseline.y + 3);
		DeleteObject(brush);

	}
	void PaintAcc(HDC hdc, Game& game) {
		long long acc = game.calCulateAcc();
		sprintf_s(buffer, "%d.%d%%", acc / 100, acc % 100);
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, Info.acc.color);
		auto font = CreateFont(Info.score.size * 1.5, Info.score.size * 0.8, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("DejaVu Sans Mono"));
		SelectObject(hdc, font);
		RECT rect = Info.acc.rect;
		DrawText(hdc, buffer, -1, &rect, DT_CENTER | DT_VCENTER);
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);
	}
	int cal_size(long dif) {
		return 5 + (1 + (Info.judge.square - dif * dif) / 6000);
	}
	void PaintHitted(HDC hdc, int x, int y, int no) {
		const int wr = 100, hr = 70;
		POINT p[2][8] = {
			{{x - wr, y}, {x - wr / 6, y - hr / 3}, {x + wr / 6, y - hr}, {x + wr / 2, y - hr / 3}, 
			{x + wr, y}, {x + wr / 6, y + hr / 3}, {x - wr / 3, y + hr}, {x - wr / 2, y + hr / 3}},

			{{x - wr * 8 / 9, y}, {x - wr / 8, y - hr / 3}, {x + wr / 3, y - hr}, {x + wr * 3 / 4, y - hr / 3},
			{x + wr * 8 / 9, y}, {x + wr / 8, y + hr / 3}, {x - wr / 3, y + hr}, {x - wr * 3 / 4, y + hr / 3}},
		};
		auto brush = CreateSolidBrush(Info.hit.hittedcolor);
		SelectObject(hdc, brush);
		Polygon(hdc, p[no], 8);
		DeleteObject(brush);
	}
	void PaintCombo(HDC hdc, Game& game, long long now) {
		if (game.stat.code == game.stat.end || game.combo < Info.minComboToShow) {
			return;
		}
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, Info.combo.color);
		auto font = CreateFont(Info.combo.size * 3, Info.combo.size * 1.7, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
		SelectObject(hdc, font);
		sprintf_s(buffer, "%d", game.combo);
		RECT rect = Info.combo.rect;
		DrawText(hdc, buffer, -1, &rect, DT_CENTER | DT_VCENTER);
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);
	}
	void PaintJudgeStat(HDC hdc, PaintEvent &pe, long long now) {
		long dif = pe.time - now;
		if (pe.kind < 0) {
			return;
		}
		if (dif < -Info.judge.additional_time) {
			return;
		}
		auto stat = Info.judge.kind[pe.kind];
		int siz = min(cal_size(dif), cal_size(stat.size));
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, stat.color);
		auto font = CreateFont(siz * 5, siz * 3, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
		SelectObject(hdc, font);
		RECT rect = Info.judge.rect;
		DrawText(hdc, stat.str.c_str(), -1, &rect, DT_CENTER | DT_VCENTER);
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);
	}
	void PaintTrackHitInfo(HDC hdc, Game& game, long long now) {
		for (int i = 0; i < 4; i++) {
			auto& track = game.track[i];
			if (!track.pressed()) continue;
			int l = Info.track.left(i);
			int r = l + Info.track.width;
			int mid = (l + r) / 2;
			int y = Info.baseline.y;
			POINT p[3] = { {mid, y + 10}, {mid - 30, y + 40}, {mid + 30, y + 40} };
			auto brush = CreateSolidBrush(Info.press.color);
			SelectObject(hdc, brush);
			Polygon(hdc, p, 3);
			DeleteObject(brush);
			if (track.phitted.time >= now) {
				int l = Info.track.left(i), r = l + Info.track.width;
				int y = Info.baseline.y;
				int mid = (l + r) / 2;
				if (now - track.phitted.lastTime > Info.hit.time) {
					track.phitted.sizeLevel = 1 - track.phitted.sizeLevel;
					track.phitted.lastTime = now;
				}
				PaintHitted(hdc, mid, y, track.phitted.sizeLevel);
			}
		}
	}
	void PaintGameProccess(HDC hdc, Game& game, long long now) {
		int per = now * Info.timeLine.length / game.totlen;
		auto brush = CreateSolidBrush(Info.timeLine.base_color);
		
		SelectObject(hdc, brush);
		Rectangle(hdc, Info.timeLine.x, Info.timeLine.y, Info.timeLine.x + Info.timeLine.length
								, Info.timeLine.y + Info.timeLine.high);
		DeleteObject(brush);
		if (game.stat.code == game.stat.end) {
			brush = CreateSolidBrush(Info.timeLine.finish_color);
		} else {
			brush = CreateSolidBrush(Info.timeLine.passed_color);
		}
		SelectObject(hdc, brush);
		Rectangle(hdc, Info.timeLine.x, Info.timeLine.y, Info.timeLine.x + per, 
									Info.timeLine.y + Info.timeLine.high);
		DeleteObject(brush);
	}
	void PaintInfo(HDC hdc, Game& game, long long now) {
		PaintTrackHitInfo(hdc, game, now);
		PaintCombo(hdc, game, now);
		PaintJudgeStat(hdc, game.pjudgestat, now);
		PaintScore(hdc, game);
		PaintAcc(hdc, game);
		if (game.stat.code == game.stat.replay) {
			PaintReplayInfo(hdc);
		}
	}
	void PaintReplayInfo(HDC hdc) {
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, minfo.default_color);
		auto font = CreateFont(28, 14, 0, 0, 100, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));
		SelectObject(hdc, font);
		RECT rect = minfo.helper.rect;
		std::string info = "press 'q' to back to MainManu";
		DrawText(hdc, info.c_str(),
			-1, &rect, DT_LEFT | DT_VCENTER);

		DeleteObject(font);
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
	}
	void PaintPause(HDC hdc) {
		auto brush = CreateSolidBrush(Info.pause.round);
		SelectObject(hdc, brush);
		Ellipse(hdc, Info.pause.x - Info.pause.r, Info.pause.y - Info.pause.r,
			Info.pause.x + Info.pause.r, Info.pause.y + Info.pause.r);
		DeleteObject(brush);
		brush = CreateSolidBrush(Info.pause.centerTri);
		SelectObject(hdc, brush);
		POINT p[3] = { {Info.pause.x - Info.pause.r / 3, Info.pause.y - Info.pause.r / 2},
				{Info.pause.x + Info.pause.r / 2, Info.pause.y},
				{Info.pause.x - Info.pause.r / 3, Info.pause.y + Info.pause.r / 2} };
		Polygon(hdc, p, 3);
		DeleteObject(brush);

		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, minfo.default_color);
		auto font = CreateFont(28, 14, 0, 0, 100, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));
		SelectObject(hdc, font);
		RECT rect = minfo.helper.rect;
		DrawText(hdc, "press 'q' to back to MainManu\npress 'p' to continue\npress 'r' to restart current song",
			-1, &rect, DT_LEFT | DT_VCENTER);
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);
	}
	void PaintScore(HDC hdc, Game& game) {
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, Info.score.color);
		auto font = CreateFont(Info.score.size * 1.6, Info.score.size, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Droid Sans Mono"));
		SelectObject(hdc, font);
		sprintf_s(buffer, "%d", game.score);
		RECT rect = Info.score.rect;
		DrawText(hdc, buffer, -1, &rect, DT_CENTER | DT_VCENTER);
		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);

	}
	void PaintEnd(HDC hdc, Game& game) {
		int oldmode = SetBkMode(hdc, TRANSPARENT);
		auto old = SetTextColor(hdc, minfo.default_color);
		auto font = CreateFont(28, 14, 0, 0, 100, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));
		SelectObject(hdc, font);
		RECT rect = minfo.helper.rect;
		std::string info = "press 'q' to back to MainManu\npress 'r' to restart current map";
		if (game.record.size() != 0) info += "\npress 'c' to watch the replay";
		DrawText(hdc, info.c_str(),
			-1, &rect, DT_LEFT | DT_VCENTER);

		DeleteObject(font);
		rect = Info.statCount.rect;
		font = CreateFont(Info.statCount.w, Info.statCount.h, 0, 0, 100, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
		SelectObject(hdc, font);
		rect.top += Info.statCount.d;
		rect.bottom += Info.statCount.d;
		SetTextColor(hdc, Info.judge.perfect.color);
		sprintf_s(buffer, "Perfect: %d", game.judgeCounter.perfect);
		DrawText(hdc, buffer, -1, &rect, DT_LEFT);

		rect.top += Info.statCount.d;
		rect.bottom += Info.statCount.d;
		SetTextColor(hdc, Info.judge.great.color);
		sprintf_s(buffer, "Great: %d", game.judgeCounter.great);
		DrawText(hdc, buffer, -1, &rect, DT_LEFT);

		rect.top += Info.statCount.d;
		rect.bottom += Info.statCount.d;
		SetTextColor(hdc, Info.judge.good.color);
		sprintf_s(buffer, "Good: %d", game.judgeCounter.good);
		DrawText(hdc, buffer, -1, &rect, DT_LEFT);

		rect.top += Info.statCount.d;
		rect.bottom += Info.statCount.d;
		SetTextColor(hdc, Info.judge.miss.color);
		sprintf_s(buffer, "Miss: %d", game.judgeCounter.miss);
		DrawText(hdc, buffer, -1, &rect, DT_LEFT);
		
		rect.top += Info.statCount.d;
		rect.bottom += Info.statCount.d;
		SetTextColor(hdc, Info.judge.miss.color);
		sprintf_s(buffer, "Max Combo: %d", game.judgeCounter.maxCombo);
		DrawText(hdc, buffer, -1, &rect, DT_LEFT);

		SetTextColor(hdc, old);
		SetBkMode(hdc, oldmode);
		DeleteObject(font);
	}
} gamePainter;