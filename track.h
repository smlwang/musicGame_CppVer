#pragma once
#include "note.h"
#include "gameinfo.h"
#include "PaintEvent.h"
#include <iostream>
class Track {

    bool keyPress = false;
    bool keyPrecheck = false;
    bool keyHold = false;
public:

    PaintEvent phitted{ 0, 0, 0, 0 };
    bool pressed() {
        return keyPress || keyHold;
    }
    
    std::vector<note> track;
    
    Track(std::vector<note>&q) : track(q) {
        
    }
    
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
    
    // gameTime ������Ϸ��ʼʱ��
    int judge(long gameTime) {
        judgeKeyChange(); // �������仯
        if (track.size() == 0) return Info.noAct;
        note &note = track.back();
        // δ���¶�Ӧ�����ж��Ƿ� miss
        if (!pressed()) {
            if (track.back().dead) {
                track.pop_back();
                return Info.perfect;
            }
            if (note.ed - gameTime < -Info.greatJudge) {
                track.pop_back();
                return Info.miss;
            }
            return Info.noAct;
        }

        // deltaTime ���ж�ʱ��Ĳ�
        long deltaTime = note.st - gameTime;
        // û��ǰ�в��ж�
        if (deltaTime > Info.goodJudge) {
            return Info.noAct;
        }
        // click �ж�
        if (note.kind == 1) { 
            if (keyHold) return Info.noAct;
            track.pop_back();
            return clickJudge(deltaTime);
        }
        
        // �����ж�
        if (note.ed < gameTime + Info.truePerfect && note.dead) {
            track.pop_back();
            return Info.perfect;
        }
        // ����β���ж����Ͽ�
        if (note.ed < gameTime - Info.perfectJudge) {
            track.pop_back();
            return Info.miss;
        }
        
        //����β��
        if (note.preCheck && pressed()) {
            if (note.ed < gameTime + Info.longEdJudge){
                note.dead = true;
                return Info.noAct;
            }
        }

        //����ͷ��
        if (!note.preCheck) {
            if (keyHold) return Info.noAct;

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