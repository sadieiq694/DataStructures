#ifndef GROBS_H
#define GROBS_H

#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <sstream>
#include <random>

// graphical objects

class QPainter;
class QPointF;
class QPaintDevice;

class Window;
class Widget;

enum class Key
{
    Left = 17,
    Right = 18,
    Up = 19,
    Down = 20
};

enum class EvtType
{
    MousePress,
    MouseRelease,
    MouseMove,
    KeyPress,
    KeyRelease,
};

enum class ModKey
{
    Shift = 1 << 0,
    Alt   = 1 << 1,
    Ctrl  = 1 << 2
};

namespace mssm
{
    class Color
    {
    public:
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a{255};
    public:
        constexpr Color()  : r(0), g(0), b(0), a(255) {}
        constexpr Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255)  : r(_r), g(_g), b(_b), a(_a) {}
    };

    constexpr Color TRANSPARENT(0,0,0,0);
    constexpr Color WHITE(255,255,255);
    constexpr Color BLACK(0,0,0);
    constexpr Color RED(255,0,0);
    constexpr Color GREEN(0,255,0);
    constexpr Color BLUE(0,0,255);
    constexpr Color YELLOW(255,255,0);
    constexpr Color PURPLE(255,0,255);
    constexpr Color CYAN(0,255,255);

    class Point
    {
    public:
        double x;
        double y;
    };


    class Event
    {
    public:
        EvtType evtType;
        int     x;
        int     y;
        ModKey  mods;
        int     arg;
    };

    class Grob;

    class Graphics
    {
    private:
        std::random_device randDevice;
        std::mt19937 mersenneTwister;
        std::function<void (Graphics&)> mainFunc;
        std::mutex glock;
        std::condition_variable cv;
        std::vector<std::unique_ptr<Grob>> queued;
        std::vector<std::unique_ptr<Grob>> active;
        std::vector<Event> _events;
        std::vector<Event> _cachedEvents;
        std::thread drawThread;
        std::string title;
        bool        closeOnExit{false};
        bool        closed{false};
        bool        finished{false};
        bool        isDrawn{false};
        bool        cleared{false};
        std::chrono::milliseconds::rep start_time;
        int         _width{100};
        int         _height{100};
        mssm::Color background;
        std::vector<bool> isPressed;
        std::vector<bool> wasPressed;
        std::string stringOutput;
        std::function<std::string()> getInputText;
    public:
        Graphics(std::string title, std::function<void (Graphics&)> mainFunc);
       ~Graphics();

        void handleEvent(int x, int y, EvtType evtType, ModKey mods, int arg);
    public:
        std::istream in;
        std::ostream out;

        int    width()  { return _width; }
        int    height() { return _height; }

        void   setBackground(Color c) { background = c; }
        void   line(double x1, double y1, double x2, double y2, Color c = WHITE);
        void   ellipse(double x, double y, double w, double h, Color c = WHITE, Color f = TRANSPARENT);
        void   polygon(std::vector<Point> pts, Color border, Color fill);
        void   polyline(std::vector<Point> pts, Color color);
        void   text(double x, double y, double size, const std::string& str, Color textColor = WHITE);

        void   clear();
        bool   draw(int delay = 0);

        void   setCloseOnExit(bool close = true) { closeOnExit = close; }
        bool   isClosed();
        void   waitUntilClosed();

        std::chrono::milliseconds::rep time();

        bool isKeyPressed(char c) { return isPressed[c]; }
        bool isKeyPressed(Key k);

        const  std::vector<Event>& events();

        int    randomInt(int minVal, int maxVal);
        double randomDouble(double minVal, double maxVal);

    private:
        void draw(QPaintDevice *pd, QPainter *painter, int width, int height, int elapsed);
        std::string getTitle() { return title; }
        void setClosed();
        void callMainFunc(Window *window);
        void addGrob(Grob* ptr);

        std::string getOutputText();
        bool appendOutputText(const std::string& txt);
        void setInputTextFunc(std::function<std::string()> func) { getInputText = func; }

        friend void helper(Graphics* g, Window *window);
        friend class ::Widget;
        friend class ::Window;
    };
}

#endif // GROBS_H
