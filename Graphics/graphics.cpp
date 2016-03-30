#include "graphics.h"
#include "window.h"

#include <iostream>

#include <functional>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <random>

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QApplication>
#include <QSurfaceFormat>
#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QPointF>
#include <QPlainTextEdit>
#include <QLineEdit>

constexpr int DRAW_WIDTH = 1000;
constexpr int DRAW_HEIGHT = 500;

using namespace mssm;
using namespace std;

#include <streambuf>

// todo:  http://stackoverflow.com/questions/5642063/inheriting-ostream-and-streambuf-problem-with-xsputn-and-overflow

class iStreamBuf : public std::streambuf
{
    public:
        explicit iStreamBuf(std::function<std::string ()> reader, std::size_t buff_sz = 256, std::size_t put_back = 8);

        iStreamBuf(const iStreamBuf &) = delete;
        iStreamBuf &operator= (const iStreamBuf &) = delete;

    private:
        // overrides base class underflow()
        int_type underflow();

        size_t readMore(char *dst, size_t toRead);

    private:
        std::function<std::string ()> _reader;
        const std::size_t _put_back;
        std::vector<char> _buffer;
        std::string _source;
};

using std::size_t;

iStreamBuf::iStreamBuf(std::function<std::string ()> reader,
                       size_t buff_sz, size_t put_back) :
    _reader{reader},
    _put_back(std::max(put_back, size_t(1))),
    _buffer(std::max(buff_sz, _put_back) + _put_back)
{
    auto end = &_buffer.front() + _buffer.size();
    setg(end, end, end);
}

size_t iStreamBuf::readMore(char *dst, size_t toRead)
{
    if (_source.empty())
    {
        _source = _reader();
    }

    size_t count = std::min(toRead, _source.size());

    if (count == 0)
    {
        return 0;
    }

    std::copy(_source.begin(), _source.begin()+count, dst);

    _source.erase(_source.begin(), _source.begin()+count);

    return count;
}

std::streambuf::int_type iStreamBuf::underflow()
{
    if (gptr() < egptr())
    {
        return traits_type::to_int_type(*gptr());
    }

    char *base = &_buffer.front();
    char *start = base;

    if (eback() == base) // true when this isn't the first fill
    {
        // Make arrangements for putback characters
        std::copy(egptr()-_put_back, egptr(), base);
//        std::memmove(base, egptr() - put_back_, put_back_);
        start += _put_back;
    }

    // start is now the start of the buffer, proper.
    // Read from fptr_ in to the provided buffer
    size_t n = readMore(start, _buffer.size() - (start - base));
    if (n == 0)
    {
        return traits_type::eof();
    }

    // Set buffer pointers
    setg(base, start, start + n);

    return traits_type::to_int_type(*gptr());
}


class oStreamBuf : public std::streambuf
{
    public:
        explicit oStreamBuf(std::function<bool (const std::string& str)> writer, std::size_t buff_sz = 256);

        oStreamBuf(const iStreamBuf &) = delete;
        oStreamBuf &operator= (const iStreamBuf &) = delete;

    protected:
        bool reallyWrite();

    private:
        int_type overflow(int_type ch);
        int sync();

    private:
        std::function<bool (const std::string& str)> _writer;
        std::vector<char> _buffer;
};

oStreamBuf::oStreamBuf(std::function<bool (const std::string& str)> writer, std::size_t buff_sz) :
    _writer(writer),
    _buffer(buff_sz + 1)
{
    char *base = &_buffer.front();
    setp(base, base + _buffer.size() - 1); // -1 to make overflow() easier
}

oStreamBuf::int_type oStreamBuf::overflow(int_type ch)
{
    if (ch != traits_type::eof())
    {
//        std::assert(std::less_equal<char *>()(pptr(), epptr()));
        *pptr() = ch;
        pbump(1);
        if (reallyWrite())
        {
            return ch;
        }
    }

    return traits_type::eof();
}

int oStreamBuf::sync()
{
    return reallyWrite() ? 0 : -1;
}

bool oStreamBuf::reallyWrite()
{
    std::ptrdiff_t n = pptr() - pbase();

    pbump(-n);

    string str(pbase(), n);

    return _writer(str);
}

namespace mssm
{
    void helper(Graphics* g, Window *window)
    {
        g->callMainFunc(window);
    }

    class Grob
    {
    public:
        virtual ~Grob() {}
        virtual void draw(QPainter *painter) = 0;
    };

    class GrobLine : public Grob
    {
        double x1;
        double y1;
        double x2;
        double y2;
        Color  c;
    public:
        GrobLine(double _x1, double _y1, double _x2, double _y2, Color _c)
            : x1(_x1), y1(_y1), x2(_x2), y2(_y2), c(_c) {}
        virtual void draw(QPainter *painter);
    };

    class GrobEllipse : public Grob
    {
        double   x;
        double   y;
        double   w;
        double   h;
        Color    c;
        Color    fill;
    public:
        GrobEllipse(double _x, double _y, double _w, double _h, Color _c, Color _fill = TRANSPARENT)
            : x(_x), y(_y), w(_w), h(_h), c(_c), fill(_fill) {}
        virtual void draw(QPainter *painter);
    };

    class GrobPoly : public Grob
    {
        std::vector<QPointF> points;
        Color    c;
        Color    fill;
        bool     closed;
    public:
        GrobPoly(std::vector<Point> points, Color outlineColor, bool closed, Color fillColor = TRANSPARENT);
        virtual void draw(QPainter *painter);
    };

    class GrobText : public Grob
    {
        double x;
        double y;
        double sz;
        std::string text;
        Color  color;
    public:
        GrobText(double x, double y, double sz, const std::string& text, Color textColor);
        virtual void draw(QPainter *painter);
    };
}

Graphics::Graphics(std::string title, std::function<void (Graphics&)> mainFunc)
    : mersenneTwister(randDevice()),
      in{new iStreamBuf([this]() { return getInputText(); })},
      out{new oStreamBuf([this](const std::string& txt) { return appendOutputText(txt); })}
{   
    mersenneTwister.seed((std::chrono::system_clock::now().time_since_epoch()).count()); // should'nt be necesary... bug in GCC?

    _width = DRAW_WIDTH;
    _height = DRAW_HEIGHT;

    this->title = title;
    background = BLACK;

    this->mainFunc = mainFunc;

    start_time =
        std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();

    int argc = 0;

    QApplication app(argc, nullptr);

    QSurfaceFormat fmt;
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    isPressed.resize(128);
    wasPressed.resize(128);

    Window window(this, getTitle());

    window.show();

    drawThread = std::thread(helper, this, &window);

    app.exec();

    setClosed();

    drawThread.join();
}

Graphics::~Graphics()
{
}

bool Graphics::isKeyPressed(Key k)
{
    return isKeyPressed((int)k);
}

int Graphics::randomInt(int minVal, int maxVal)
{
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(mersenneTwister);
}

double Graphics::randomDouble(double minVal, double maxVal)
{
    std::uniform_real_distribution<double> dist(minVal, maxVal);
    return dist(mersenneTwister);
}

void Graphics::handleEvent(int x, int y, EvtType evtType, ModKey mods, int arg)
{
    Event evt{evtType,x,y,mods,arg};

    std::unique_lock<std::mutex> lock(glock);

    if (closed)
    {
        return;
    }

    switch (evtType)
    {
    case EvtType::KeyPress:
        if (arg < 128)
        {
            isPressed[arg]  = true;
        }
        else
        {
            switch (arg)
            {
            case Qt::Key_Left:  isPressed[(int)Key::Left]  = true; evt.arg = (int)Key::Left;  break;
            case Qt::Key_Right: isPressed[(int)Key::Right] = true; evt.arg = (int)Key::Right; break;
            case Qt::Key_Up:    isPressed[(int)Key::Up]    = true; evt.arg = (int)Key::Up;    break;
            case Qt::Key_Down:  isPressed[(int)Key::Down]  = true; evt.arg = (int)Key::Down;  break;
            }
        }
        break;
    case EvtType::KeyRelease:
        if (arg < 128)
        {
            isPressed[arg] = false;
        }
        else
        {
            switch (arg)
            {
            case Qt::Key_Left:  isPressed[(int)Key::Left]  = false; evt.arg = (int)Key::Left;  break;
            case Qt::Key_Right: isPressed[(int)Key::Right] = false; evt.arg = (int)Key::Right; break;
            case Qt::Key_Up:    isPressed[(int)Key::Up]    = false; evt.arg = (int)Key::Up;    break;
            case Qt::Key_Down:  isPressed[(int)Key::Down]  = false; evt.arg = (int)Key::Down;  break;
            }
        }
        break;
    default:
        break;
    }

    _events.emplace_back(std::move(evt));
}

void Graphics::callMainFunc(Window *window)
{
    mainFunc(*this);

    if (closeOnExit)
    {
        if (!isClosed())
        {
            QMetaObject::invokeMethod( window, "close");
        }
    }
    else
    {
        draw();
        waitUntilClosed();
    }
}

void Graphics::setClosed()
{
    std::unique_lock<std::mutex> lock(glock);

    closed  = true;

    cv.notify_all();
}

bool Graphics::isClosed()
{
    return closed;
}

void Graphics::waitUntilClosed()
{
    std::unique_lock<std::mutex> lock(glock);

    cv.wait(lock, [this]{ return closed; });
}

void Graphics::draw(QPaintDevice *pd, QPainter *painter, int width, int height, int /*elapsed*/)
{
    _width = width;
    _height = height;

    std::unique_lock<std::mutex> lock(glock);

    if (finished)
    {
        if (cleared)
        {
            active.swap(queued);
        }
        else
        {
            int oldSize = active.size();
            active.resize(active.size() + queued.size());
            std::swap_ranges(active.begin()+oldSize, active.end(), queued.begin());
        }
        queued.clear();
    }

    if (!closed)
    {
        painter->begin(pd);
        painter->setRenderHint(QPainter::Antialiasing);

        QBrush qbg = QBrush(QColor(background.r, background.g, background.b));

        painter->fillRect(0, 0, width, height, qbg);

        for (auto& g : active)
        {
            g->draw(painter);
        }

        painter->end();
    }

    if (finished)
    {
        finished = false;
        isDrawn = true;
    }

    cv.notify_all();
}

const std::vector<Event>& Graphics::events()
{
    std::unique_lock<std::mutex> lock(glock);

    if (closed)
    {
        _cachedEvents.clear();
        return _cachedEvents;
    }

    if (_cachedEvents.size() == 0 && _events.size() > 0)
    {
        _cachedEvents = _events;
    }

    return _cachedEvents;
}

std::string Graphics::getOutputText()
{
    std::unique_lock<std::mutex> lock(glock);

    if (closed)
    {
        return "";
    }

    string tmp = stringOutput;

    stringOutput.clear();

    return tmp;
}

bool Graphics::appendOutputText(const std::string& txt)
{
    std::unique_lock<std::mutex> lock(glock);

    if (closed)
    {
        return false;
    }

    stringOutput.append(txt);

    return true;
}


bool Graphics::draw(int minSleepTime)
{
    std::chrono::system_clock::time_point endTime;

    if (minSleepTime)
    {
        endTime = std::chrono::system_clock::now() + std::chrono::milliseconds(minSleepTime);
    }

    {
        std::unique_lock<std::mutex> lock(glock);

        if (_events.size() > 0)
        {
            _events.clear();
            _cachedEvents.clear();
        }


        finished = true;

        cv.wait(lock, [this]{ return isDrawn || closed; });

        isDrawn = false;

        if (closed)
        {
            return false;
        }
    }

    if (minSleepTime)
    {
        std::this_thread::sleep_until(endTime);
    }

    return true;
}


void Graphics::clear()
{
    std::unique_lock<std::mutex> lock(glock);
    cleared = true;
}

void Graphics::addGrob(Grob *ptr)
{
    std::unique_ptr<Grob> grob(ptr);
    std::unique_lock<std::mutex> lock(glock);
    if (closed)
    {
        return;
    }
    queued.emplace_back(std::move(grob));
}

void Graphics::line(double x1, double y1, double x2, double y2, Color c)
{
    addGrob(new GrobLine{x1,y1,x2,y2,c});
}

void Graphics::ellipse(double x, double y, double w, double h, Color c, Color f)
{
    addGrob(new GrobEllipse{x,y,w,h,c,f});
}

void Graphics::polygon(std::vector<Point> pts, Color border, Color fill)
{
    addGrob(new GrobPoly(pts, border, true, fill));
}

void Graphics::polyline(std::vector<Point> pts, Color color)
{
    addGrob(new GrobPoly(pts, color, false));
}

void Graphics::text(double x, double y, double size, const std::string &str, Color textColor)
{
    addGrob(new GrobText(x,y,size,str,textColor));
}

std::chrono::milliseconds::rep Graphics::time()
{
    auto milliseconds_since_epoch =
        std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();

    return milliseconds_since_epoch - start_time;
}

GrobText::GrobText(double x, double y, double sz, const std::string& text, Color textColor)
{
    this->x = x;
    this->y = y;
    this->sz = sz;
    this->text = text;
    this->color = textColor;
}

void GrobText::draw(QPainter *painter)
{
    QFont f("Arial");
    f.setPixelSize(sz);
    painter->setFont(f);
    painter->setPen(QColor{color.r, color.g, color.b, color.a});
    painter->drawText(x, y, QString(text.c_str()));
}

GrobPoly::GrobPoly(std::vector<Point> points, Color outlineColor, bool closed, Color fillColor)
{
    this->points.resize(points.size());

    for (unsigned int i=0;i<points.size();i++)
    {
        this->points[i].setX(points[i].x);
        this->points[i].setY(points[i].y);
    }

    c = outlineColor;
    fill = fillColor;
    this->closed = closed;
}

void GrobPoly::draw(QPainter *painter)
{
    painter->setPen(QColor{c.r, c.g, c.b, c.a});
    if (closed)
    {
        painter->setBrush(QBrush(QColor{fill.r, fill.g, fill.b, fill.a}));
        painter->drawPolygon(&points[0], points.size());
    }
    else
    {
        painter->drawPolyline(&points[0], points.size());
    }
}


void GrobLine::draw(QPainter *painter)
{

    painter->setPen(QColor{c.r, c.g, c.b, c.a});
    painter->drawLine(x1, y1, x2, y2);
}

void GrobEllipse::draw(QPainter *painter)
{
    painter->setPen(QColor{c.r, c.g, c.b, c.a});
    painter->setBrush(QBrush(QColor{fill.r, fill.g, fill.b, fill.a}));
    painter->drawEllipse(x, y, w, h);
}

Widget::Widget(mssm::Graphics *graphics, Window *parent)
    : QWidget(parent), _graphics(graphics), _parent(parent)
{
    elapsed = 0;

    setMinimumHeight(DRAW_HEIGHT);
    setMinimumWidth(DRAW_WIDTH);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void Widget::animate()
{
    elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) / 1000;
    update();
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter;

    _graphics->draw(this, &painter, event->rect().width(), event->rect().height(), elapsed);

    auto str = _graphics->getOutputText();
    if (!str.empty())
    {
        _parent->appendOutputText(str);
    }
}

ModKey cvtMods(Qt::KeyboardModifiers qmods)
{
    ModKey mods = static_cast<ModKey>(
            ((qmods & Qt::ControlModifier) ? (int)ModKey::Ctrl : 0) |
            ((qmods & Qt::AltModifier) ? (int)ModKey::Alt : 0) |
            ((qmods & Qt::ShiftModifier) ? (int)ModKey::Shift : 0));

    return mods;
}

void Widget::mousePressEvent(QMouseEvent * event)
{
    _graphics->handleEvent(event->x(), event->y(), EvtType::MousePress,
                 cvtMods(event->modifiers()),
                 (int)event->button());
}

void Widget::mouseReleaseEvent(QMouseEvent * event)
{
    _graphics->handleEvent(event->x(), event->y(), EvtType::MouseRelease,
                 cvtMods(event->modifiers()),
                 (int)event->button());
}

void Widget::mouseMoveEvent(QMouseEvent * event)
{
    _graphics->handleEvent(event->x(), event->y(), EvtType::MouseMove,
                 cvtMods(event->modifiers()),
                 0);
}

void Widget::keyPressEvent(QKeyEvent * event)
{
    _graphics->handleEvent(0, 0, EvtType::KeyPress,
                 cvtMods(event->modifiers()),
                 event->key());
}

void Widget::keyReleaseEvent(QKeyEvent * event)
{
    _graphics->handleEvent(0,0, EvtType::KeyRelease,
                 cvtMods(event->modifiers()),
                 event->key());
}

Window::Window(mssm::Graphics *g, std::string title)
{
    graphics = g;

    g->setInputTextFunc([this]() { return getInputText(); });

    setWindowTitle(tr(title.c_str()));

    graphicsWidget = new Widget(graphics, this);
    streamOutBox = new QPlainTextEdit;
    streamInBox = new QLineEdit;

    QGridLayout *layout = new QGridLayout;

    layout->setRowStretch(0,1);
    layout->setRowStretch(1,0);
    layout->setRowStretch(2,0);

    layout->addWidget(graphicsWidget, 0, 0);
    layout->addWidget(streamOutBox, 1, 0);
    layout->addWidget(streamInBox, 2, 0);

    streamInBox->hide();
    streamOutBox->hide();
    streamInBox->setEnabled(false);

    connect(streamInBox, SIGNAL(returnPressed()), this, SLOT(textEntered()));

    setLayout(layout);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), graphicsWidget, SLOT(animate()));
    timer->start(17);
}

void Window::appendOutputText(const std::string& txt)
{
    if (graphics->isClosed())
    {
        return;
    }

    if (streamOutBox->isHidden())
    {
        streamOutBox->show();
    }

    streamOutBox->moveCursor(QTextCursor::End);
    streamOutBox->insertPlainText(QString::fromStdString(txt));
    streamOutBox->moveCursor(QTextCursor::End);
}

std::string Window::getInputText()
{
    if (graphics->isClosed())
    {
        return "";
    }

    if (!usingStreamIO)
    {
        usingStreamIO = true;
        QMetaObject::invokeMethod( streamInBox, "show");
    }

    QMetaObject::invokeMethod( streamInBox, "setEnabled", Q_ARG(bool, true));
    QMetaObject::invokeMethod( streamInBox, "setFocus");

    std::unique_lock<std::mutex> lock(wlock);

    cv.wait(lock, [this]{ return hasEnteredText; });

    string str;

    if (hasEnteredText)
    {
        std::swap(str, enteredText);
        hasEnteredText = false;
    }

    return str;
}

void Window::textEntered()
{
    auto str = streamInBox->text().toStdString();

    streamInBox->clear();

    std::unique_lock<std::mutex> lock(wlock);

    enteredText.append(str);
    enteredText.append("\n");

    hasEnteredText = true;

    streamInBox->setEnabled(false);
    graphicsWidget->setFocus();

    cv.notify_all();
}

