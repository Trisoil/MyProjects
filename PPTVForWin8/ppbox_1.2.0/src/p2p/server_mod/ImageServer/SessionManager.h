#pragma once

struct Session
{
};

class SessionManager
{
public:
    SessionManager(void);
public:
    ~SessionManager(void);
public:
    Session * get_session(size_t key);
private:
    std::map<size_t, Session> sessions_;
};
