#ifndef BASE_DISPLAY
#define BASE_DISPLAY
#include <functional>
class IDisplay {
   public:
    virtual void whiteOnBlack();
    virtual void blackOnWhite();
    virtual void print(const char* line);
    virtual void print(char c);
    virtual void print(unsigned char uc);
    virtual void print(int i);
    virtual void print(unsigned int ui);
    virtual void print(long l);
    virtual void print(unsigned long ul);
    virtual void print(double d);   
    virtual void eol();
};

inline void displayPrintline(IDisplay* display)
{
  display->eol();
}
template <typename T, typename... Types> 
inline void displayPrintline(IDisplay* display, T var1, Types... var2)
{
  display->print(var1);
  displayPrintline(display, var2...);
}

typedef std::function<void(IDisplay *display)> TDrawFunction;

#endif