#include <iostream>
#include <glib.h>
#include <Windows.h>
#include "GlibTest.h"

using namespace std;

static void handle_connect_thread(gpointer data) {
    cout << "start" << endl;
    bool* bRunning = (bool*)data;
    *bRunning = true;
    cout << "end" << endl;
}

int main() {
    cout << "hi" << endl;

    static bool bconnectThreadRunning = false;
    cout << "bool " << bconnectThreadRunning << endl;
    if (!bconnectThreadRunning)
    {
        bconnectThreadRunning = false;
        cout << "bool " << bconnectThreadRunning << endl;
        g_thread_new("handle_connect_thread", (GThreadFunc)handle_connect_thread, &bconnectThreadRunning);
    }

    Sleep(2000);
    cout << "bool " << bconnectThreadRunning << endl;
    getchar();
}