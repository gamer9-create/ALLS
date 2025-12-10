//
// Created by lalit on 11/16/2025.
//

#ifndef ALLS_CLIENT_H
#define ALLS_CLIENT_H
#include <string>
#include <unordered_map>
#include "Player.h"

void StartClient(std::string IPAddress = "127.0.0.1", int Port = 5000);
void StopClient();

std::unordered_map<long, Player> *GetPlayers();
void UpdateState(PlayerState state);
double GetServerTime();

#endif //ALLS_CLIENT_H