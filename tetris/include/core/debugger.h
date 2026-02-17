#pragma once

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <string>

namespace dbg
{
void start(const std::string& filename);
void stop();
void log(std::string msg);
} // namespace dbg

#endif // DEBUGGER_H
