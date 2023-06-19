#pragma once
#include <fstream>
#include "track.h"
#include <vector>
#include "PaintEvent.h"
#include "note.h"
#include "SongItem.h"
#include "MusicPlayer.h"

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
		tracks[track - 1].push_back({ type, st, ed});
	}
	file.close();
	for (auto& v : tracks) {
		std::reverse(v.begin(), v.end());
	}
	return tracks;
}
struct Game {
	struct keyEvent {
		int trackNo;
		int event; // 1 key press, 0 key up;
		long long time; // event time
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
	PaintEvent pjudgestat{ -1, 0, 0 };
	std::vector<keyEvent> record;
	std::vector<Track> track;
	double speed = 1;

	void speedup() {
		if (speed < 2) speed += 0.05;
	}
	void speeddown() {
		if (speed > 0.6) speed -= 0.05;
	}
	long long calCulateAcc() {
		long long delta = judgeCounter.miss * Info.acc_miss + judgeCounter.good * Info.acc_good + judgeCounter.great * Info.acc_great;
		long long all = judgeCounter.curAllNote() * Info.acc_miss;
		if (all == 0) return 0;
		return 10000ll * (all - delta) / all;
	}
	struct counter {
		int perfect;
		int great;
		int good;
		int miss;
		int maxCombo;
		int curAllNote() {
			return perfect + great + good + miss;
		}
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
	}
	void Start(SongItem _song) {
		song = _song;
		auto tracks = read(song.map_path.c_str());
		for (auto& v : tracks) {
			track.emplace_back(v);
		}
		musicPlayer.openMusic(song.song_path);
		totlen = musicPlayer.getCurrentSongLength();
		stat.code = stat.pause;
	}

	void Replay() {
		auto tmp = record;
		initGame();
		record = tmp;
		std::reverse(record.begin(), record.end());
		Start(song);
		pause();
	}
	void replayKeyPress(long long now) { 
		while (record.size()) {
			auto& r = record.back();
			if (r.time > now) {
				return;
			}
			track[r.trackNo].keyChange(r.event);
			record.pop_back();
		}
	}
	
	void keyChange(int trackNo, int event, long long now) {
		record.push_back({ trackNo, event, now });
		track[trackNo].keyChange(event);
	}


	bool isend(long long now) {
		return now >= totlen;
	}
	void EndGame() {
		if (stat.code != stat.notReady) {
			musicPlayer.closeMusic();
		}
	}

	void pause() {
		if (stat.code == stat.pause) {
			musicPlayer.resumeMusic();
			stat.code = stat.run;
		} else if (stat.code == stat.run) {
			musicPlayer.pauseMusic();
			stat.code = stat.pause;
		}
	}

	void judge(long long now) {
		if (stat.code != stat.run && stat.code != stat.replay) return;
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
			}
		}
		if (stat >= 0 && stat <= 3) {
			pjudgestat = {stat, now + Info.judge.anime_time, 0};
		}
	}
};