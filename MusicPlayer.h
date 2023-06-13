#pragma once
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <iostream>
struct MusicPlayer {
    char buffer[1024];
    void MymciSendString(const char *szCommand, char *szbuffer) {
        if (NULL == szbuffer) {
            mciSendString(szCommand, NULL, 0, NULL);
        } else {
            mciSendString(szCommand, szbuffer, 1024, NULL);
        }
    }
    long long getCurrentSongLength() {
        MymciSendString("status gmusic length", buffer);
        return atol(buffer);
    }
    void openMusic(std::string &path) {
        std::string cmd = "open " + path + " alias gmusic";
        MymciSendString(cmd.c_str(), NULL);
    }
    void playMusic() { MymciSendString("play gmusic", NULL); }
    long long currentTime() {
        mciSendString("status gmusic position", buffer, 512, NULL);
        return atol(buffer);
    }
    void closeMusic() { MymciSendString("close gmusic", NULL); }
    void resumeMusic() { MymciSendString("resume gmusic", NULL); }
    void pauseMusic() { MymciSendString("stop gmusic", NULL); }
} musicPlayer;