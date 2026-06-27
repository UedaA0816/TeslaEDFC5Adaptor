#pragma once

#include "DIContainer.h"

class IFeature {
  public:
    explicit IFeature(DIContainer& diContainer) : _diContainer(diContainer) {}
    virtual ~IFeature() = default;
    virtual void setup() = 0;
    virtual void loop() = 0;

  protected:
    DIContainer& diContainer() { return _diContainer; }

  private:
    DIContainer& _diContainer;
};
