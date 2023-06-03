#pragma once
#include <fstream>
#include "track.h"
#include <vector>
#include "PaintEvent.h"
#include <Windows.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")
#include "note.h"
#include "SongItem.h"
char buffer[1024];
std::vector<std::vector<note>> read(const char *path) {
	std::fstream file(path, std::ios::in);
	std::vector<std::vector<note>> tracks(4);
	int type, track;
	long st, ed;
	while (file >> type) {
		file >> track;
		file >> st;
		if (type == 1) { // single note
			ed = st;
		} else { // long note
			file >> ed;
		}
		tracks[track - 1].push_back({ type, st, ed });
	}
	file.close();
	for (auto& v : tracks) {
		std::reverse(v.begin(), v.end());
	}
	return tracks;
}
void MymciSendString(const char* szCommand, char* szbuffer) {
	if (NULL == szbuffer) {
		if (0 != mciSendString(szCommand, NULL, 0, NULL)) {
			printf("%s false!\n", szCommand);
		} else {
			printf("%s success!\n", szCommand);
		}
	} else {
		if (0 != mciSendString(szCommand, szbuffer, 1024, NULL)) {
			printf("%s false!\n", szCommand);
		} else {
			printf("%s success!\n", szCommand);
		}
	}
}
struct Game {
	struct keyEvent {
		int trackNo;
		int event; // 1 key press, 0 key up;
		long time; // event time
	};

	struct Stat {
		const int notReady = 0;
		const int run = 1;
		const int pause = 2;
		const int end = 4;
		const int replay = 8;
		int code = 0;
	} stat;
	int combo = 0, score = 0;
	long totlen = 0;
	SongItem song;
	PaintEvent pjudgestat{ -1, 0, 0 }, pcombo{ 0, 0, 0 };
	std::vector<keyEvent> record;
	std::vector<Track> track;

	struct counter {
		int perfect;
		int great;
		int good;
		int miss;
		int maxCombo;
	} judgeCounter;

	void initGame() {
		track = {};
		record = {};
		totlen = 0;
		combo = 0;
		score = 0;
		stat.code = 0;
		judgeCounter = {};
		pjudgestat = { -1, 0, 0, 0 };
		pcombo = {};
	}
	void GameStart(SongItem _song) {
		song = _song;
		auto tracks = read(song.map_path.c_str());
		for (auto& v : tracks) {
			track.emplace_back(v);
		}
		std::string song_open = "open ";
		song_open += song.song_path + " alias gmusic";
		MymciSendString(song_open.c_str(), NULL);
		MymciSendString("status gmusic length", buffer);
		totlen = atoi(buffer);
		MymciSendString("play gmusic", NULL);
		stat.code = stat.run;
	}

	void Replay() {
		auto tmp = record;
		initGame();
		record = tmp;
		std::reverse(record.begin(), record.end());
		GameStart(song);
	}
	void replayKeyPress() {
		const int offset = 3;
		long now = time();
		while (record.size()) {
			auto& r = record.back();
			if (r.time > now) {
				return;
			}
			track[r.trackNo].keyChange(r.event);
			record.pop_back();
		}
	}
	void keyChange(int trackNo, int event) {
		record.push_back({ trackNo, event, time() });
		track[trackNo].keyChange(event);
	}
	
	long time() {
		mciSendString("status gmusic position", buffer, 512, NULL);
		return atol(buffer);
	}
	bool isend() {
		return time() >= totlen;
	}
	
	void EndGame() {
		if (stat.code != stat.notReady) {
			MymciSendString("close gmusic", NULL);
		}
	}

	void pause() {
		if (stat.code == stat.run || stat.code == stat.pause) {
			if (stat.code == stat.pause) {
				MymciSendString("resume gmusic", NULL);
				stat.code = stat.run;
			} else if (stat.code == stat.run) {
				MymciSendString("stop gmusic", NULL);
				stat.code = stat.pause;
			}
		}
	}

	void judge() {
		if (stat.code != stat.run && stat.code != stat.replay) return;
		long now = time();
		int stat = 4;
		for (auto& t : track) {
			int tmp = t.judge(now);
			if (tmp == Info.miss) judgeCounter.miss += 1;
			else if (tmp == Info.good) judgeCounter.good += 1;
			else if (tmp == Info.great) judgeCounter.great += 1;
			else if (tmp == Info.perfect) judgeCounter.perfect += 1;
			if (tmp >= 1 && tmp <= 3) {
				score += tmp * 10;
			}
			int hitted = tmp >= Info.good && tmp <= Info.longHold;
			if (hitted) {
				t.phitted.kind = 1;
				t.phitted.time = now + Info.hit.anime_time;
			}
			if (tmp == Info.noAct) continue;
			if (stat >= 0 && tmp < stat) stat = tmp;
			if (tmp == 0) combo = 0;
			else if (stat <= 3) {
				combo += 1;
				judgeCounter.maxCombo = max(judgeCounter.maxCombo, combo);
				pcombo = { combo, now, 0 };
			}
		}
		if (stat >= 0 && stat <= 3) {
			pjudgestat = {stat, now + Info.judge.anime_time, 0};
		}
	}
};