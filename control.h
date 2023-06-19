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
	long long now = 0;
	void updateTime() {
		if (scence == scode.game) {
			now = musicPlayer.currentTime();
		} else {
			now = 0;
		}
	}
	int scence = scode.mainView;
	void check() {
		if (scence == scode.game) {
			if (game.isend(now)) {
				game.stat.code = game.stat.end;
			}
			if (game.stat.code == game.stat.replay) {
				game.replayKeyPress(now);
			}
			game.judge(now);
		}
	}
	void passKey(char c, int event) {
		if (c == 'o') {
			game.speedup();
		} else if (c == 'i') {
			game.speeddown();
		}
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
				game.keyChange(track, event, now);
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
		scence = scode.game;
		game.initGame();
		game.Start(mw.songList.curSong());
	}
	void paint(HDC hdc) {
		if (scence == scode.mainView) {
			mainViewPainter.PaintMainView(hdc, mw);
		} else if (scence == scode.game) {
			gamePainter.PaintGame(hdc, game, now);
		}
	}
};