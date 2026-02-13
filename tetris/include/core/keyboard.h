#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <functional>
#include <unistd.h>

#include "core/ansi.h"

class Keyboard
{
  public:
    void listen(std::function<std::function<void()>(ansi::Key)> callback);

  private:
    bool holding = false;
    std::function<void()> releaseCallback;
};

#endif // KEYBOARD_H
