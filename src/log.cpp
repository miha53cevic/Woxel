#include "log.h"

LOG::LOG()
{
    fopen_s(&m_file, "./logs.txt", "w");
}

LOG LOG::get()
{
    static LOG log;
    return log;
}

void LOG::write(std::string message)
{
    fprintf(m_file, "%s\n", message.c_str());
}

void LOG::close()
{
    fclose(m_file);
}
