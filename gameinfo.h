#pragma once
#include <Windows.h>
#include <iostream>
struct GameINFO {

    //判定区间 ms
    const int goodJudge = 150;
    const int greatJudge = 90;
    const int longEdJudge = 75;
    const int perfectJudge = 50;
    const int truePerfect = 30;

    const int acc_miss = 1000;
    const int acc_good = 600;
    const int acc_great = 300;

    //判定结果
    const int noAct = -1;
    const int miss = 0;
    const int good = 1;
    const int great = 2;
    const int perfect = 3;
    const int longHold = 4;

    const int holdJudgeCnt = 3;
    const int pressCancelCnt = 3;
    const int showJudge = 300; // ms
    const int early = 1;
    const int mid = 2;
    const int late = 3;
    const int eShift = 10;
    const int lShift = 20;
    const int minComboToShow = 3;

    // 画画
    struct Baseline {
        const COLORREF color = RGB(128, 53, 53);
        const int l = 150, r = 650;
        const int y = 700;
    } baseline;
    struct TrackShift {
        const int offset = 200;
        const int width = 100;
        int left(int idx) const {
            return offset + width * idx;
        }
    } track;
    struct Press {
        const COLORREF color = RGB(5, 83, 205);
    } press;
    struct Note {
        const int click = 1;
        const int hold = 2;
        const int high = 45;
        const int adjust = 5;
        const int bhigh = 10;
        const int perfectHigh = 2;
        const COLORREF white = RGB(255, 255, 255);
        const COLORREF blue = RGB(0, 255, 255);
        const COLORREF longNote = RGB(248, 244, 207);
        const COLORREF perfectJudge = RGB(245, 204, 37);
    } note;
    struct Combo {
        // const int x = 400;
        // const int y = 300;
        const RECT rect = { 260, 300, 560, 500 };
        const int size = 40;
        const int anime_time = 5;
        const COLORREF color = RGB(186, 163, 20);
    } combo;

    struct JudgeStat {
        const int anime_time = 200, square = anime_time * anime_time;
        const int additional_time = 100;
        const RECT rect = {300, 170, 550, 300};
        // const int x = 300; 
        // const int y = 200;
        struct stat {
            int size;
            COLORREF color;
            std::string str;
        };
        stat good {
            5,
            RGB(11, 189, 227),
            "Good",
        }, great  {
            5,
            RGB(4, 186, 22),
            "Great",
        }, perfect {
            5,
            RGB(255, 138, 0),
            "Perfect",
        }, miss {
            10,
            RGB(182, 23, 23),
            "Miss!",
        };
        const stat kind[4] = { miss, good, great, perfect };
    } judge;
    struct hit {
        const int anime_time = 100;
        const COLORREF color = RGB(186, 163, 20);
        const COLORREF hittedcolor = RGB(255, 247, 37);
        int time = 50;
    } hit;
    struct pause {
        const int x = 400, y = 350;
        const int r = 50;
        const COLORREF centerTri = RGB(18, 191, 217);
        const COLORREF round = RGB(35, 191, 74);
    } pause;

    struct score {
        const RECT rect = { 650, 100, 900, 200 }; 
        const int size = 30;
        const COLORREF color = RGB(255, 155, 155);

    } score;
    struct acc {
        const RECT rect = { 650, 200, 900, 300 };
        const int size = 15;
        const COLORREF color = RGB(222, 220, 40);
    } acc;
    struct statCount {
        const int w = 60, h = 35;
        const int d = 80;
        const RECT rect = { 100, 150, 600, 260 };
    } statCount;
    struct timeLine {
        const int x = 720;
        const int y = 160;
        const int length = 100;
        const int high = 10;
        const COLORREF passed_color = RGB(123, 210, 8);
        const COLORREF base_color = RGB(100, 100, 100);
        const COLORREF finish_color = RGB(120, 0, 60);
    } timeLine;
};
const static GameINFO Info;