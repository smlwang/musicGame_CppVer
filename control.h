#pragma once
#include "Key.h"
#include "mainView.h"
#include "mainViewPaint.h"
#include "game.h"
#include "gamePaint.h"
struct Controller {
	struct scenceCode {
		const int mainView = 0, game = 1;
	} scode;
	MainView mw{"songlist.ini"};
	Game game;
	int scence = 0;
	void check() {
		if (scence == scode.game) {
			if (game.isend()) {
				game.stat.code = game.stat.end;
			}
			if (game.stat.code == game.stat.replay) {
				game.replayKeyPress();
			}
			game.judge();
		}
	}
	void passKey(char c, int event) {
		if (scence == scode.mainView) {
			if (event == 1 && (c == 'j' || c == 'k')) {
				mw.changeSong(c);
			} else if (c == 's') {
				newGage();
			}
		} else if (scence == scode.game) {
			int track = -1;
			if (c == 'd') track = 0;
			else if (c == 'f') track = 1;
			else if (c == 'j') track = 2;
			else if (c == 'k') track = 3;
			if (track != -1 && game.stat.code != game.stat.replay) {
				game.keyChange(track, event);
				return;
			}
			if (c == 'p' && game.stat.code != game.stat.replay) {
				game.pause();
				return;
			}
			if (game.stat.code == game.stat.replay) {
				if (c == 'q') {
					game.EndGame();
					scence = scode.mainView;
				}
				return;
			}
			if ((game.stat.code & (game.stat.pause | game.stat.end)) != 0) {
				if (c == 'q') {
					game.EndGame();
					scence = scode.mainView;
				} else if (c == 'r') {
					game.EndGame();
					newGage();
				}
			}
			if (game.stat.code == game.stat.end) {
				if (c == 'c' && game.record.size()) {
					game.EndGame();
					game.Replay();
					game.stat.code = game.stat.replay;
				}
			}
		}
	}
	void newGage() {
		scence = 1;
		game.initGame();
		game.GameStart(mw.songList.curSong());
	}
	void paint(HDC hdc) {
		if (scence == scode.mainView) {
			mainViewPainter.PaintMainView(hdc, mw);
		} else if (scence == scode.game) {
			gamePainter.PaintGame(hdc, game);
		}
	}
};