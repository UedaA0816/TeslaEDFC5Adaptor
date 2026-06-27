#pragma once

class IGateway {
  public:
    virtual ~IGateway() = default;
    virtual void start() = 0;
    virtual void update() = 0;
};
