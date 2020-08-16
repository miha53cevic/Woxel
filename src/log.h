#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <string>

class LOG
{
public:
    LOG();

    static LOG get();

    void write(std::string message);
    void close();

private:
    FILE* m_file;
};

#endif