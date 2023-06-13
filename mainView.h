#pragma once
#include "SongItem.h"
#include <fstream>
#include <iostream>
#include <vector>

struct MainView {
    struct SongList {
        std::vector<SongItem> songs;
        SongItem &operator[](int idx) { return songs[idx]; }
        int idx = 0;
        int size() { return songs.size(); }
        void nextSong() {
            idx += 1;
            if (idx >= songs.size())
                idx = 0;
        }
        void preSong() {
            idx -= 1;
            if (idx == -1)
                idx = songs.size() - 1;
        }
        SongItem curSong() { return songs[idx]; }
        SongList(std::string path) {
            std::fstream file{path};
            std::string name, song, map;
            while (file >> name >> song >> map) {
                songs.push_back({name, song, map});
            }
            file.close();
        }
    } songList;
    void changeSong(char c) {
        if (c == 'j') {
            songList.nextSong();
        } else if (c == 'k') {
            songList.preSong();
        }
    }
    MainView(std::string path) : songList(path) {}
};