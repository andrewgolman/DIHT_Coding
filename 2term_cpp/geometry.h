#include <iostream>
#include <vector>
#include <cmath>
#include <typeinfo>

using std::vector;

const double EPS = 1e-7;
class Point;
class Line;

bool ifEqual (double a, double b = 0) {
    return a - b < EPS && b - a < EPS;
}

double ellipticIntegral (double bigAxis, double eccSq) {
    double smallAxis = sqrt(bigAxis*bigAxis*(1-eccSq));
    return M_PI * (3*(bigAxis + smallAxis) - sqrt((3*bigAxis + smallAxis)*(bigAxis + 3*smallAxis)));
}

/*double ellipticIntegral (double bigAxis, double eccSq) {
    double res = 0;
    for (double t = 0; t < M_PI/2; t += M_PI/2/10000000) {
        res += sqrt(1 - eccsq * cos(t) * cos(t));
    }
    res /= 10000000;
    return res * 4 * bigAxis;
}*/

struct Point {
public:
    double x, y;
    Point() : x(0), y(0) {}
    Point(double a, double b) : x(a), y(b) {}
    bool operator == (const Point& b) const {
        return ifEqual(x, b.x) && ifEqual(y, b.y);
    }
    bool operator != (const Point& b) const {
        return !ifEqual(x, b.x) || !ifEqual(y, b.y);
    }
    void reflex(const Point& center) {
        x = 2 * center.x - x;
        y = 2 * center.y - y;
    }
    void rotate(const Point& center, double angle) {
        double xCopy = x;
        x = (x - center.x) * cos(angle) - (y - center.y) * sin(angle) + center.x;
        y = (xCopy - center.x) * sin(angle) + (y - center.y) * cos(angle) + center.y;
    }
    void scale(const Point& center, double coefficient) {
        x = (x - center.x) * coefficient + center.x;
        y = (y - center.y) * coefficient + center.y;
    }
    double distance(const Point& b) const {
        return sqrt((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y));
    }
    const Point segmentMiddle(const Point& b) const {
        return Point((x + b.x)/2, (y + b.y)/2);
    }
    double squareUnderSegment(const Point& b) const {
        return (b.x - x) * (y + b.y) / 2;
    }
};

class Line {
public:
    double xCoef, yCoef;
    double shift;
public:
    Line (const double a, const double b) : xCoef(a), yCoef(-1), shift(b) {}
    Line (const Point& p, const double a) {
        xCoef = a;
        yCoef = -1;
        shift = p.y - xCoef * p.x;
    }
    Line (const Point& p, const Point& q) {
        if (ifEqual(q.x, p.x)) {
            xCoef = 1;
            yCoef = 0;
            shift = -q.x;
        } else {
            xCoef = (q.y - p.y) / (q.x - p.x);
            yCoef = -1;
            shift = -yCoef * p.y - xCoef * p.x;
        }
    }
    Line (const Point& p, const Line& line) {
        if (line.xCoef == 0)
            *this = Line(p, Point(p.x, p.y + 1));
        else {
            xCoef = line.yCoef / line.xCoef;
            *this = Line(p, xCoef);
        }

    }
    bool operator == (const Line& line) const {
        return ifEqual(xCoef * line.yCoef, line.xCoef * yCoef) && ifEqual(yCoef * line.shift, line.yCoef * shift);
    }
    bool operator != (const Line& line) const {
        return !ifEqual(xCoef * line.yCoef, line.xCoef * yCoef) || !ifEqual(yCoef * line.shift, line.yCoef * shift);
    }
    bool ifIntersect(const Line& line) const {
        return !ifEqual(xCoef * line.yCoef - yCoef * line.xCoef);
    }
    const Point intersect(const Line& line) const {
        double determinant = xCoef * line.yCoef - yCoef * line.xCoef;
        double xCoord = (-shift * line.yCoef + yCoef * line.shift) / determinant;
        double yCoord = (-xCoef * line.shift + shift * line.xCoef) / determinant;
        return Point(xCoord, yCoord);
    }
    void reflex(Point& point) const {
        if (yCoef == 0) {
            point.x = -2 * shift - point.x;
        } else {
            point.rotate(Point(), atan(xCoef / yCoef));
            point.y = -2 * shift - point.y;
            point.rotate(Point(), atan(-xCoef / yCoef));
        }
    }
};

double getSin(const Point& a, const Point& b, const Point& c) {
    double len1 = a.distance(b), len2 = b.distance(c);
    return ((b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x)) / len1 / len2;
}
double getCos(const Point& a, const Point& b, const Point& c) {
    double l1 = a.distance(b), l2 = b.distance(c);
    return ((b.x - a.x) * (c.x - b.x) + (b.y - a.y) * (c.y - b.y)) / l1 / l2;
}

class Shape {
public:
    virtual bool operator == (const Shape& another) const = 0;
    virtual bool operator != (const Shape& another) const = 0;
    virtual bool isCongruentTo(const Shape& another) const = 0;
    virtual bool isSimilarTo(const Shape& another) const = 0;
    virtual bool containsPoint(const Point& point) const = 0;
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual void rotate(const Point& center, double angle) = 0;
    virtual void reflex(const Point& center) = 0;
    virtual void reflex(const Line& axis) = 0;
    virtual void scale(const Point& center, double coefficient) = 0;
    virtual ~Shape() {}
};

class Polygon : public Shape {
protected:
    vector<Point> vertex;
public:
    Polygon(const vector<Point>& points) {
        vertex.resize(points.size());
        for (size_t i = 0; i < points.size(); ++i)
            vertex[i] = points[i];
    }
    template <class... Args>
    void init(const Point& p, Args... args) {
        vertex.push_back(p);
        init(args...);
    }
    void init() {}
    template <class... Args>
    Polygon(Args... args) {
        init(args...);
    };
    size_t verticesCount() const {
        return vertex.size();
    }
    vector<Point> getVertices() const {
        return vertex;
    }
    bool isConvex() const {
        bool positiveSin = false, negativeSin = false;
        size_t n = vertex.size();
        for (size_t i = 0; i < n; ++i)
            if (getSin(vertex[i], vertex[(i+1) % n], vertex[(i+2) % n]) > 0)
                positiveSin = true;
            else if (getSin(vertex[i], vertex[(i+1) % n], vertex[(i+2) % n]) < 0)
                negativeSin = true;
        return !(positiveSin && negativeSin);
    }
    double perimeter() const {
        double per = 0;
        for (size_t i = 0; i < vertex.size(); ++i)
            per += vertex[i].distance(vertex[(i+1) % vertex.size()]);
        return per;
    }
    double area() const {
        double ar = 0;
        for (size_t i = 0; i < vertex.size(); ++i)
            ar += vertex[i].squareUnderSegment(vertex[(i+1) % vertex.size()]);
        return ar >= 0 ? ar : -ar;
    }
    bool operator == (const Shape& another) const {
        Polygon anotherPol;
        try {
            anotherPol = dynamic_cast<const Polygon&>(another);
        }
        catch (std::bad_cast) {
            return false;
        }
        if (verticesCount() != anotherPol.verticesCount())
            return false;
        for (size_t i = 0; i < vertex.size(); ++i) {
            bool equal = true;
            for (size_t j = 0; equal && j < vertex.size(); ++j) {
                if (vertex[j] !=  anotherPol.vertex[(i+j) % vertex.size()])
                    equal = false;
            }
            if (equal)
                return true;
        }
        for (size_t i = 0; i < vertex.size(); ++i) {
            bool equal = true;
            for (size_t j = 0; equal && j < vertex.size(); ++j) {
                if (vertex[j] !=  anotherPol.vertex[(i-j+vertex.size()) % vertex.size()])
                    equal = false;
            }
            if (equal)
                return true;
        }
        return false;
    }
    bool operator != (const Shape& another) const {
        return !(*this == another);
    }
    bool roundPass(bool congruent, bool straight, const Polygon& another) const {
        if (verticesCount() != another.verticesCount())
            return false;
        for (size_t i = 0; i < vertex.size(); ++i) {
            int way = straight ? 1 : -1;
            size_t n = vertex.size();
            bool equal = true;
            bool sameSin = false;
            bool oppSin = false;
            double ratio = 0;
            for (size_t j = 0; equal && j < n; ++j) {
                if (congruent && !ifEqual(vertex[j].distance(vertex[(j + 1) % n]),
                                          another.vertex[(i + j*way + n) % n].distance(another.vertex[(i + (j+1) * way + n) % n])))
                    equal = false;
                else if (!congruent && ifEqual(ratio, 0))
                    ratio = vertex[j].distance(vertex[(j+1) % n]) /
                            another.vertex[(i + j*way + n) % n].distance(another.vertex[(i + (j+1) * way + n) % n]);
                else if (!congruent && !ifEqual(vertex[j].distance(vertex[(j+1) % n]) /
                                                another.vertex[(i + j*way + n) % n].distance(another.vertex[(i + (j+1) * way + n) % n]), ratio))
                    equal = false;
            }
            for (size_t j = 0; equal && j < n; ++j) {
                if (!ifEqual(getCos(vertex[j], vertex[(j + 1) % n], vertex[(j + 2) % n]),
                             getCos(another.vertex[(i + j*way + n) % n], another.vertex[(i + (j+1) * way + n) % n],
                                    another.vertex[(i + (j+2) * way + n) % n])))
                    equal = false;
                double sin1 = getSin(vertex[j], vertex[(j + 1) % n], vertex[(j + 2) % n]);
                double sin2 = getSin(another.vertex[(i + j*way + n) % n], another.vertex[(i + (j+1) * way + n) % n],
                                     another.vertex[(i + (j+2) * way + n) % n]);
                if (ifEqual(sin1, sin2))
                    sameSin = true;
                else if (ifEqual(sin1, -sin2) && !ifEqual(sin1, 0))
                    oppSin = true;
                else
                    equal = false;
            }
            if (!(sameSin && oppSin) && equal)
                return true;
        }
        return false;
    }
    bool isCongruentTo(const Shape& another) const {
        Polygon anotherPol;
        try {
            anotherPol = dynamic_cast<const Polygon&>(another);
        }
        catch (std::bad_cast) {
            return false;
        }
        return roundPass(true, true, anotherPol) || roundPass(true, false, anotherPol);
    }
    bool isSimilarTo(const Shape& another) const {
        Polygon anotherPol;
        try {
            anotherPol = dynamic_cast<const Polygon&>(another);
        }
        catch (std::bad_cast b) {
            return false;
        }
        return roundPass(false, true, anotherPol) || roundPass(false, false, anotherPol);
    }
    bool containsPoint(const Point& point) const {
        bool oddIntersections = 0;
        size_t n = vertex.size();
        Point righty(point.x + 1, point.y);
        Line partitionLine(point, righty);
        for (size_t i = 0; i < n; ++i) {
            if (!Line(vertex[i], vertex[(i + 1) % n]).ifIntersect(partitionLine))
                continue;
            double xIntersection = Line(vertex[i], vertex[(i + 1) % n]).intersect(partitionLine).x;
            if (ifEqual(xIntersection, point.x) &&
                ((xIntersection >= vertex[i].x && xIntersection <= vertex[(i + 1) % n].x) ||
                 (xIntersection <= vertex[i].x && xIntersection >= vertex[(i + 1) % n].x))) {
                return true;
            }
            if (xIntersection > point.x && ((vertex[i].y >= point.y) ^ (vertex[(i + 1) % n].y >= point.y)) &&
                ((xIntersection >= vertex[i].x && xIntersection <= vertex[(i + 1) % n].x) ||
                 (xIntersection <= vertex[i].x && xIntersection >= vertex[(i + 1) % n].x))) {
                oddIntersections = !oddIntersections;
            }
        }
        return oddIntersections;
    }

    void rotate(const Point& center, double angle) {
        for (size_t i = 0; i < vertex.size(); ++i)
            vertex[i].rotate(center, angle);
    }
    void reflex(const Point& center) {
        for (size_t i = 0; i < vertex.size(); ++i)
            vertex[i].reflex(center);
    }
    void reflex(const Line& axis) {
        for (size_t i = 0; i < vertex.size(); ++i)
            axis.reflex(vertex[i]);
    }
    void scale(const Point& center, double coefficient) {
        for (size_t i = 0; i < vertex.size(); ++i)
            vertex[i].scale(center, coefficient);
    }
};

class Ellipse : public Shape {
protected:
    Point foc1, foc2;
    double bigAxis;
public:
    Ellipse(const Point& f1, const Point& f2, const double d) : foc1(f1), foc2(f2), bigAxis(d/2) {}
    const std::pair<Point,Point> focuses() const {
        return std::make_pair(foc1, foc2);
    }
    const std::pair<Line, Line> directrixes() const {
        Point paxis1 = foc1;
        paxis1.scale(center(), bigAxis*bigAxis/foc1.distance(foc2) / foc1.distance(foc2) * 4);
        Point paxis2 = foc2;
        paxis1.scale(center(), bigAxis*bigAxis/foc1.distance(foc2) / foc1.distance(foc2) * 4);
        return std::make_pair(Line(paxis1, Line(foc1, foc2)), Line(paxis2, Line(foc1, foc2)));
    }
    double eccentricity() const {
        return focus()/bigAxis;
    }
    double focus() const {
        return foc1.distance(foc2) / 2;
    }
    Point center() const {
        return foc1.segmentMiddle(foc2);
    }
    bool operator == (const Shape& another) const {
        Ellipse anotherEl(Point(), Point(), 0);
        try {
            anotherEl = dynamic_cast<const Ellipse&>(another);
        }
        catch (std::bad_cast) {
            return false;
        }
        return center() == anotherEl.center() && ifEqual(bigAxis, anotherEl.bigAxis);
    }
    bool operator != (const Shape& another) const {
        return !(*this == another);
    }
    bool isCongruentTo(const Shape& another) const {
        Ellipse anotherEl(Point(), Point(), 0);
        try {
            anotherEl = dynamic_cast<const Ellipse&>(another);
        }
        catch (std::bad_cast) {
            return false;
        }
        return ifEqual(focus(), anotherEl.focus()) && ifEqual(bigAxis, anotherEl.bigAxis);
    }
    bool isSimilarTo(const Shape& another) const {
        Ellipse anotherEl(Point(), Point(), 0);
        try {
            anotherEl = dynamic_cast<const Ellipse&>(another);
        }
        catch (std::bad_cast) {
            return false;
        }
        return ifEqual(focus()/bigAxis, anotherEl.focus()/anotherEl.bigAxis);
    }
    bool containsPoint(const Point& point) const {
        return point.distance(foc1) + point.distance(foc2) < 2*bigAxis + EPS;
    }
    double perimeter() const {
        return ellipticIntegral(bigAxis, eccentricity()*eccentricity());
    }
    double area() const {
        return M_PI * bigAxis * sqrt(bigAxis * bigAxis - focus() * focus());
    }
    void rotate(const Point& center, double angle) {
        foc1.rotate(center, angle);
        foc2.rotate(center, angle);
    }
    void reflex(const Point& center) {
        foc1.reflex(center);
        foc2.reflex(center);
    }
    void reflex(const Line& axis) {
        axis.reflex(foc1);
        axis.reflex(foc2);
    }
    void scale(const Point& center, double coefficient) {
        foc1.scale(center, coefficient);
        foc2.scale(center, coefficient);
        bigAxis *= coefficient;
    }
};

class Circle : public Ellipse {
public:
    Circle(const Point& center, const double rad) : Ellipse(center, center, 2*rad) {}
    double radius() const {
        return bigAxis;
    }
};

class Rectangle : public Polygon {
public:
    Rectangle(const Point& v1, const Point& v2, const Point& v3, const Point& v4)  : Polygon(v1, v2, v3, v4) {}
    Rectangle(const Point& v1, const Point& v3, double ratio) {
        if (ratio > 1)
            ratio = 1/ratio;
        Point v2(v3.x, v3.y), v4(v3.x, v3.y);
        v2.rotate(v1, -atan(ratio));
        v2.scale(v1, cos(atan(ratio)));
        v4.rotate(v1, atan(1/ratio));
        v4.scale(v1, sin(atan(ratio)));
        *this = Rectangle(v1, v2, v3, v4);
    }
    const Point center() const {
        return vertex[0].segmentMiddle(vertex[2]);
    }
    const std::pair<Line, Line> diagonals() const {
        return std::make_pair(Line(vertex[0], vertex[2]), Line(vertex[1], vertex[3]));
    }
};

class Square : public Rectangle {
public:
    Square(const Point& a, const Point& c) : Rectangle(a, Point((a.x + c.x + a.y - c.y)/2, (-a.x + c.x + a.y + c.y)/2),
                c, Point((a.x + c.x - a.y + c.y)/2, (a.x - c.x + a.y + c.y)/2)) {}
    const Circle circumscribedCircle() const {
        return Circle(vertex[0].segmentMiddle(vertex[2]), vertex[0].distance(vertex[2])/2);
    }
    const Circle inscribedCircle() const {
        return Circle(vertex[0].segmentMiddle(vertex[2]), vertex[0].distance(vertex[1])/2);
    }
};

class Triangle : public Polygon {
public:
    Triangle(const Point& v1, const Point& v2, const Point& v3) : Polygon(v1, v2, v3) {}
    const Circle circumscribedCircle() const {
        Line midPerp1(vertex[0].segmentMiddle(vertex[1]), Line(vertex[0], vertex[1]));
        Line midPerp2(vertex[0].segmentMiddle(vertex[2]), Line(vertex[0], vertex[2]));
        Point center = midPerp1.intersect(midPerp2);
        return Circle(center, vertex[0].distance(center));
    }
    const Circle inscribedCircle() const {
        double tangent = (vertex[0].distance(vertex[1]) + vertex[0].distance(vertex[2])
                          - vertex[1].distance(vertex[2])) / 2;
        Point v1 = vertex[1], v2 = vertex[2];
        v1.scale(vertex[0], tangent / vertex[0].distance(vertex[1]));
        v2.scale(vertex[0], tangent / vertex[0].distance(vertex[2]));
        Line r1(v1, Line(vertex[0], vertex[1]));
        Line r2(v2, Line(vertex[0], vertex[2]));
        Point center = r1.intersect(r2);
        return Circle(center, area() / perimeter() * 2);
    }
    const Point centroid() const {
        double xsum = vertex[0].x + vertex[1].x + vertex[2].x;
        double ysum = vertex[0].y + vertex[1].y + vertex[2].y;
        return Point(xsum/3, ysum/3);
    }
    const Point orthocenter() const {
        Line height1(vertex[1], Line(vertex[0], vertex[2]));
        Line height2(vertex[2], Line(vertex[0], vertex[1]));
        return height1.intersect(height2);
    }
    const Line EulerLine() const {
        return Line(orthocenter(), circumscribedCircle().center());
    }
    const Circle ninePointsCircle() const {
        Point center = orthocenter().segmentMiddle(circumscribedCircle().center());
        return Circle(center, circumscribedCircle().radius() / 2);
    }
};