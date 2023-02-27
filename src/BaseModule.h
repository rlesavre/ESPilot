#ifndef BASE_MODULE
#define BASE_MODULE

#include "ScreenPage.h"

class IBaseModule {
   public:
    virtual const char* getName() = 0;
    // return null, if wedon't have a ScreenPage
    virtual IScreenPage* getDisplayPage() = 0;

    virtual void setup() = 0;
    virtual void loop() = 0;
};

#endif