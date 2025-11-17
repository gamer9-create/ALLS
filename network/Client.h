//
// Created by lalit on 11/16/2025.
//

#ifndef ALLS_CLIENT_H
#define ALLS_CLIENT_H
#include <string>

void StartClient(std::string IPAddress = "127.0.0.1", int Port = 5000);
void StopClient();
void UpdatePosition(float x, float y);

#endif //ALLS_CLIENT_H