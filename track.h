#pragma once
#include "note.h"
#include "info.h"
#include "PaintEvent.h"
#include <queue>
#include <iostream>
class Track {

    bool keyPressed = false;
    bool keyPress = false;
    bool keyHold = false;

    int holdCnt = 0;
    int pressCancel = 0;
public:
    PaintEvent phitted{ 0, 0, 0, 0 };
    bool pressed() {
        return keyPress || keyHold;
    }
    std::vector<note> track;
    Track(std::vector<note>&q) : track(q) {
        
    }

    void keyChange(bool pressed) {
        keyPressed = pressed;
        if (pressed) {
            keyPress = true;
            keyHold = false;
            pressCancel = 0;
            holdCnt = 0;
            return;
        }
        pressCancel++;
    }
    // gameTime 距离游戏开始时间

    int judge(long gameTime) {

        if (!keyPressed) {
            pressCancel++;
            if (pressCancel > 2) {
                keyReset();
            }
        }
        if (!keyPress && !keyHold) {
            keyHold = false;
            holdCnt = 0;
        }
        if (keyPress) {
            if (Info.holdJudgeCnt > holdCnt + 1) {
                holdCnt += 1;
            } else {
                holdCnt = Info.holdJudgeCnt;
            }
        }
        if (holdCnt >= Info.holdJudgeCnt) keyHold = true;
        if (track.size() == 0) return Info.noAct;
        note &note = track.back();
        // deltaTime 与判定时间的差
        if (!keyPress) {
            if (note.ed - gameTime < -Info.greatJudge) {
                track.pop_back();
                return Info.miss;
            }
            return Info.noAct;
        }
        long deltaTime = note.st - gameTime;
        if (deltaTime > Info.goodJudge) // 没到前判不判定
        {
            return Info.noAct;
        }
        if (note.kind == 1) {
            if (keyHold) return Info.noAct;
            track.pop_back();
            if (!keyPressed) {
                keyReset();
            }
            keyHold = keyPress;
            return clickJudge(deltaTime);
        } else {
            if (!keyPress || note.ed < gameTime) {
                if (note.dead) {
                    track.pop_back();
                    return Info.perfect;
                }
            }
            if (note.ed < gameTime - Info.perfectJudge) {
                track.pop_back();
                return Info.miss;
            }
            long tailJudge = -Info.greatJudge;
            if (note.preCheck && keyPress)
                tailJudge = Info.perfectJudge + 10;
            if (note.ed < gameTime + tailJudge)//长条尾判
            {
                if (note.preCheck && keyPress) {
                    note.dead = true;
                    return Info.noAct;
                }
            }
            if (!note.preCheck)//长条头判
            {
                if (keyHold) return Info.noAct;
                if (std::abs(deltaTime) <= Info.greatJudge) {
                    note.preCheck = true;
                    keyHold = keyPress;
                    return Info.perfect;
                } else if (deltaTime < -Info.greatJudge) {
                    note.preCheck = true;
                    keyHold = keyPress;
                    return Info.good;
                }
                return Info.noAct;
            }
            if (keyPressed) {
                note.st = max(gameTime, note.st);
            }
            return Info.longHold;
        }
        
    }
    int clickJudge(long deltaTime) {
        int stat;
        if (deltaTime >= 0) {
            if (deltaTime <= Info.perfectJudge)
                stat = Info.perfect;
            else if (deltaTime <= Info.greatJudge)
                stat = Info.great;
            else if (deltaTime <= Info.goodJudge)
                stat = Info.good;
            else stat = Info.miss;
        } else {
            deltaTime = -deltaTime;
            if (deltaTime <= Info.perfectJudge)
                stat = Info.perfect;
            else if (deltaTime <= Info.greatJudge)
                stat = Info.great;
            else if (deltaTime <= Info.goodJudge)
                stat = Info.good;
            else stat = Info.miss;
        }
        return stat;
    }
    void keyReset() {
        keyPress = false;
        keyHold = false;
        pressCancel = 0;
        holdCnt = 0;
    }

};