#ifndef WINDOW
#define WINDOW

#include <functional>
#include <QWidget>

#include "graphics.h"

class QPlainTextEdit;
class QLineEdit;

class Window;

class Widget : public QWidget
{
    Q_OBJECT

    mssm::Graphics *_graphics;
    Window   *_parent;

public:
    Widget(mssm::Graphics *graphics, Window *parent);

public slots:
    void animate();

protected:
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override ;
    void mouseReleaseEvent(QMouseEvent * event) override ;
    void mouseMoveEvent(QMouseEvent * event) override ;
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;
private:
    int elapsed;
};

class Window : public QWidget
{
    Q_OBJECT

    std::mutex wlock;
    std::condition_variable cv;

    Widget         *graphicsWidget;
    QPlainTextEdit *streamOutBox;
    QLineEdit      *streamInBox;
    mssm::Graphics *graphics;

    bool            hasEnteredText{false};
    std::string     enteredText;
    bool            usingStreamIO{false};

public:
    Window(mssm::Graphics *graphics, std::string title);

    void appendOutputText(const std::string& txt);
    std::string getInputText();

public slots:
    void textEntered();
};

#endif // WINDOW

