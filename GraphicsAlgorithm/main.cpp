#define _USE_MATH_DEFINES

#include "graphics.h"

#include <limits>

using namespace std;
using namespace mssm;

// if we are treating this as a directed segment, p2 is the head/arrow point
class Segment
{
public:
    Vec2d p1;
    Vec2d p2;
    Color c;

};

Vec2d randomPoint(Graphics& g)
{
    return Vec2d{g.randomDouble(1,g.width()-1), g.randomDouble(1,g.height()-1)};
}

Color randomColor(Graphics& g)
{
    return Color(g.randomInt(0,255),g.randomInt(0,255),g.randomInt(0,255) );
}

vector<Vec2d> randomPoints(Graphics& g, int numPts)
{
    vector<Vec2d> points;

    for (int i=0;i<numPts;i++)
    {
        points.push_back(randomPoint(g));
    }

    return points;
}

vector<Segment> randomSegments(Graphics& g, int count)
{
    vector<Segment> segments;

    for (int i=0;i<count;i++)
    {
        Segment s;
        s.p1 = randomPoint(g);
        s.p2 = randomPoint(g);
        s.c  = randomColor(g);
        segments.push_back(s);
    }

    return segments;
}

double distance(Vec2d p1, Vec2d p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

double distToSegment(Segment& seg, Vec2d pos, Vec2d& closestPoint)
{

}

Vec2d closestPoint(const vector<Segment>& lines, Vec2d pos)
{
    if (lines.size() == 0)
    {
        return Vec2d{0,0};
    }

    double leastDistance = distance(pos, lines[0].p1);
    Vec2d  leastPoint = lines[0].p1;

    for (int i = 0; i < lines.size(); i++)
    {
        double d1 = distance(pos, lines[i].p1);
        double d2 = distance(pos, lines[i].p2);

        if (d1 < leastDistance)
        {
            leastDistance = d1;
            leastPoint = lines[i].p1;
        }

        if (d2 < leastDistance)
        {
            leastDistance = d2;
            leastPoint = lines[i].p2;
        }
    }

    return leastPoint;
}

double crossProduct(Vec2d v1, Vec2d v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

bool isPointOnLeftSide(Segment line, Vec2d point)
{
    if (crossProduct(point - line.p1, line.p2 - line.p1) > 0)
    {
        return true;
    }
    return false;
}
double dotProduct(Vec2d v1, Vec2d v2)
{
    return v1.x*v2.x + v1.y*v2.y;
}

Vec2d closestPointOnLine(Segment& line, Vec2d pt)
{
    //first find proj of line.p1 to pt on line.p1 to line.p2
    Vec2d v1 = pt - line.p1;
    Vec2d v2 = line.p2 - line.p1;
    double magV2 = distance(line.p1, line.p2);
    double proj = dotProduct(v1, v2)/magV2;
    if(proj <= 0)
    {
        return line.p1;
    }
    else if(proj >= magV2)
    {
        return line.p2;
    }
    else
    {
        Vec2d v2UnitVec = {v2.x/magV2, v2.y/magV2};
        return proj*(v2UnitVec) + line.p1;
    }
}

double distanceToClosestPointOnLine(Segment& line, Vec2d pt, Vec2d& closestPoint)
{
    closestPoint = closestPointOnLine(line, pt);
    return distance(closestPoint,  pt);
}

void graphicsMain(Graphics& g)
{
    vector<Vec2d>   pts;   // = randomPoints(g, 1000);
    vector<Segment> lines;

    Segment newLine;
    newLine.c = WHITE;
    bool drawingNewLine = false;
    static Vec2d point;
    while (g.draw())
    {
        for (const Event& evt : g.events())
        {
            switch (evt.evtType)
            {
            case EvtType::KeyPress:
                break;
            case EvtType::KeyRelease:
                break;
            case EvtType::MouseMove:
                point = {evt.x, evt.y};
                newLine.p2 = Vec2d(evt.x, evt.y);
                break;
            case EvtType::MousePress:
                switch (evt.arg)
                {
                case 1:
                {
                    Vec2d pnt = {evt.x, evt.y};
                    if (isPointOnLeftSide(newLine, pnt))
                    {
                        g.out << "It's on the left!"<<endl;
                    }
                    else
                    {
                        g.out << "It's on the right!" << endl;
                    }
                }
                    break;
                case 2: // drawing lines
                {
                    newLine.p1 = Vec2d(evt.x, evt.y);
                    newLine.p2 = newLine.p1;
                    drawingNewLine = true;
                }
                    break;
                case 4: //distance to closest point on line
                    Vec2d pnt ={evt.x, evt.y};
                    {Vec2d closestPnt;
                    g.out << distanceToClosestPointOnLine(lines.back(), pnt, closestPnt) << endl;
                    }
                }
                break;
            case EvtType::MouseRelease:
                switch (evt.arg)
                {
                case 1:
                    break;
                case 2:
                    newLine.p2 = Vec2d(evt.x, evt.y);
                    lines.push_back(newLine);
                    drawingNewLine = false;
                    break;
                }

                break;
            }
        }


        for (Segment& s: lines)
        {
            g.line(s.p1, s.p2, s.c);
            auto p = closestPointOnLine(s, g.mousePos());
            g.ellipse(p - Vec2d{3,3}, 6, 6, CYAN);
        }

        if (drawingNewLine)
        {
            g.line(newLine.p1, newLine.p2, GREEN);
            g.ellipse(newLine.p2, 5, 5, BLUE, WHITE);
        }

        if (lines.size() > 0)
        {
            auto p = closestPoint(lines, g.mousePos());
            g.ellipse(p - Vec2d{3,3}, 6, 6);

        }


        g.points(pts, WHITE);
        g.clear();


    }
}

int main()
{
    Graphics g("Graphics App", graphicsMain);
}
