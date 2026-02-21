#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <string>
#include <unistd.h>

#include "core/ansi.h"

class Keyboard
{
  public:
    ansi::Key poll();

  private:
    std::string buffer;
};

#endif // KEYBOARD_H
