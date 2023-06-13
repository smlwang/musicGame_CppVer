#pragma once
#include "PaintEvent.h"
#include "gameinfo.h"
#include "note.h"
#include <iostream>
#include <queue>

class Track {

    bool keyPress = false;
    bool keyPrecheck = false;
    bool keyHold = false;

public:
    PaintEvent phitted{0, 0, 0, 0};
    bool pressed() { return keyPress || keyHold; }
    std::vector<note> track;
    Track(std::vector<note> &q) : track(q) {}
    void keyChange(bool keyDown) {
        if (keyDown) {
            if (!pressed()) {
                keyPress = 1;
            }
        } else {
            keyPrecheck = 0;
            keyHold = 0;
            keyPress = 0;
        }
    }
    void judgeKeyChange() {
        if (keyPress) {
            if (keyPrecheck) {
                keyPress = 0;
                keyHold = 1;
            } else {
                keyPrecheck = 1;
            }
        }
    }
    // gameTime 距离游戏开始时间

    int judge(long gameTime) {
        judgeKeyChange();
        if (track.size() == 0)
            return Info.noAct;
        note &note = track.back();
        if (!pressed()) {
            if (note.ed - gameTime < -Info.greatJudge) {
                track.pop_back();
                return Info.miss;
            }
            return Info.noAct;
        }

        // deltaTime 与判定时间的差
        long deltaTime = note.st - gameTime;
        if (deltaTime > Info.goodJudge) { // 没到前判不判定
            return Info.noAct;
        }

        if (note.kind == 1) { // click 判定
            if (keyHold)
                return Info.noAct;
            track.pop_back();
            return clickJudge(deltaTime);
        }

        // 长条判定
        if (note.dead) {
            track.pop_back();
            return Info.perfect;
        }
        // 长条尾部判定更严苛
        if (note.ed < gameTime - Info.perfectJudge) {
            track.pop_back();
            return Info.miss;
        }
        long tailJudge = -Info.greatJudge;
        if (note.preCheck && pressed()) {
            tailJudge = Info.perfectJudge + 10;
        }
        if (note.ed < gameTime + tailJudge) { // 长条尾判
            if (note.preCheck && pressed()) {
                note.dead = true;
                return Info.noAct;
            }
        }

        if (!note.preCheck) { // 长条头判
            if (keyHold)
                return Info.noAct;

            if (std::abs(deltaTime) <= Info.greatJudge) {
                note.preCheck = true;
                return Info.perfect;
            } else if (deltaTime < -Info.greatJudge) {
                note.preCheck = true;
                return Info.good;
            }
            return Info.noAct;
        }
        if (pressed()) {
            note.st = max(gameTime, note.st);
        }
        return Info.longHold;
    }

    int clickJudge(long deltaTime) {
        int clickStat = Info.miss;
        if (deltaTime < 0) {
            deltaTime = -deltaTime;
        }
        if (deltaTime <= Info.perfectJudge) {
            clickStat = Info.perfect;
        } else if (deltaTime <= Info.greatJudge) {
            clickStat = Info.great;
        } else if (deltaTime <= Info.goodJudge) {
            clickStat = Info.good;
        }
        return clickStat;
    }
};