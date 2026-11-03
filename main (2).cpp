// Shape Geometry Engine (simple version)
// Compile:  g++ -std=c++17 main.cpp -o shapes
// Run:      ./shapes

#include <algorithm>   // sort
#include <cmath>       // sqrt, fabs
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const double PI = 3.14159265358979;

// ---------------------------------------------------------------
// MODULE 3: two small interfaces (classes with only pure virtual functions)
// ---------------------------------------------------------------
class Drawable {
public:
    virtual ~Drawable() {}
    virtual void draw() const = 0;
};

class Measurable {
public:
    virtual ~Measurable() {}
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
};

// ---------------------------------------------------------------
// MODULE 1: abstract Shape. It inherits from BOTH interfaces
// (multiple inheritance), so it gets draw(), area(), perimeter().
// ---------------------------------------------------------------
class Shape : public Drawable, public Measurable {
public:
    virtual ~Shape() {}                      // virtual destructor
    virtual string name() const = 0;
};

// ---------------------------------------------------------------
// MODULE 2: concrete shapes
// ---------------------------------------------------------------
class Circle : public Shape {
private:
    int radius;
public:
    Circle(int r) { radius = r; }
    int getRadius() const { return radius; }

    double area() const { return PI * radius * radius; }
    double perimeter() const { return 2 * PI * radius; }
    string name() const { return "Circle"; }
    void draw() const;                       // written in Module 4 below
};

class Rectangle : public Shape {
private:
    int width, height;
public:
    Rectangle(int w, int h) { width = w; height = h; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    double area() const { return width * height; }
    double perimeter() const { return 2 * (width + height); }
    string name() const { return "Rectangle"; }
    void draw() const;
};

struct Point {
    double x, y;
};

class Polygon : public Shape {
private:
    vector<Point> points;                    // corners, in order
public:
    Polygon(vector<Point> p) { points = p; }
    const vector<Point>& getPoints() const { return points; }

    // Shoelace formula: add up (x1*y2 - x2*y1) for every side, halve it
    double area() const {
        double sum = 0;
        int n = points.size();
        for (int i = 0; i < n; i++) {
            int next = (i + 1) % n;          // wraps back to the first point
            sum += points[i].x * points[next].y - points[next].x * points[i].y;
        }
        return fabs(sum) / 2;
    }

    double perimeter() const {
        double sum = 0;
        int n = points.size();
        for (int i = 0; i < n; i++) {
            int next = (i + 1) % n;
            double dx = points[next].x - points[i].x;
            double dy = points[next].y - points[i].y;
            sum += sqrt(dx * dx + dy * dy);
        }
        return sum;
    }

    string name() const { return "Polygon"; }
    void draw() const;
};

// ---------------------------------------------------------------
// MODULE 4: ASCII renderer
// Each cell of the grid prints "##" (two characters, because letters
// are taller than they are wide) or two spaces.
// ---------------------------------------------------------------
void printCell(bool filled) {
    if (filled) cout << "##";
    else        cout << "  ";
}

void Circle::draw() const {
    for (int y = radius; y >= -radius; y--) {
        for (int x = -radius; x <= radius; x++) {
            printCell(x * x + y * y <= radius * radius);   // inside the circle?
        }
        cout << endl;
    }
}

void Rectangle::draw() const {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            printCell(true);
        }
        cout << endl;
    }
}

// Is the point (x, y) inside the polygon? (standard "ray" test)
bool isInside(const vector<Point>& p, double x, double y) {
    bool inside = false;
    int n = p.size();
    for (int i = 0, j = n - 1; i < n; j = i, i++) {
        bool crosses = (p[i].y > y) != (p[j].y > y);
        if (crosses) {
            double xAtY = (p[j].x - p[i].x) * (y - p[i].y) / (p[j].y - p[i].y) + p[i].x;
            if (x < xAtY) inside = !inside;
        }
    }
    return inside;
}

void Polygon::draw() const {
    // find the box that contains all the points
    double minX = points[0].x, maxX = points[0].x;
    double minY = points[0].y, maxY = points[0].y;
    for (const Point& p : points) {
        minX = min(minX, p.x);  maxX = max(maxX, p.x);
        minY = min(minY, p.y);  maxY = max(maxY, p.y);
    }
    for (double y = maxY - 0.5; y > minY; y -= 1) {
        for (double x = minX + 0.5; x < maxX; x += 1) {
            printCell(isInside(points, x, y));
        }
        cout << endl;
    }
}

// dynamic_cast: ask "what is this Shape REALLY?" and show extra details.
void showDetails(const Shape* s) {
    cout << "--- " << s->name() << " ---" << endl;

    if (const Circle* c = dynamic_cast<const Circle*>(s)) {
        cout << "Radius: " << c->getRadius() << endl;
    } else if (const Rectangle* r = dynamic_cast<const Rectangle*>(s)) {
        cout << "Size: " << r->getWidth() << " x " << r->getHeight() << endl;
    } else if (const Polygon* p = dynamic_cast<const Polygon*>(s)) {
        cout << "Corners: " << p->getPoints().size() << endl;
    }

    cout << "Area: " << s->area() << "  Perimeter: " << s->perimeter() << endl;
    s->draw();
    cout << endl;
}

// ---------------------------------------------------------------
// MODULE 5: collection manager
// ---------------------------------------------------------------
class ShapeManager {
private:
    vector<Shape*> shapes;
public:
    ~ShapeManager() {                        // free everything we own
        for (Shape* s : shapes) delete s;
    }

    void add(Shape* s) { shapes.push_back(s); }

    bool remove(int index) {
        if (index < 0 || index >= (int)shapes.size()) return false;
        delete shapes[index];
        shapes.erase(shapes.begin() + index);
        return true;
    }

    // smallest area first
    void sortByArea() {
        sort(shapes.begin(), shapes.end(), [](Shape* a, Shape* b) {
            return a->area() < b->area();
        });
    }

    int size() const { return shapes.size(); }
    Shape* get(int index) const { return shapes[index]; }

    void printAll() const {
        for (int i = 0; i < (int)shapes.size(); i++) {
            cout << i << ". " << shapes[i]->name()
                 << "  area=" << shapes[i]->area() << endl;
        }
    }
};

// ---------------------------------------------------------------
// TESTS: compare computed values with answers we know are right
// ---------------------------------------------------------------
int passed = 0, failed = 0;

void check(string what, double got, double expected) {
    if (fabs(got - expected) < 0.0001) {
        passed++;
    } else {
        failed++;
        cout << "FAILED: " << what << " (got " << got
             << ", expected " << expected << ")" << endl;
    }
}

void runTests() {
    Circle c(1);
    check("circle area", c.area(), PI);
    check("circle perimeter", c.perimeter(), 2 * PI);

    Rectangle r(3, 4);
    check("rectangle area", r.area(), 12);
    check("rectangle perimeter", r.perimeter(), 14);

    Polygon square({{0, 0}, {4, 0}, {4, 4}, {0, 4}});
    check("square area", square.area(), 16);
    check("square perimeter", square.perimeter(), 16);

    Polygon triangle({{0, 0}, {4, 0}, {0, 3}});      // 3-4-5 triangle
    check("triangle area", triangle.area(), 6);
    check("triangle perimeter", triangle.perimeter(), 12);

    ShapeManager m;
    m.add(new Rectangle(10, 10));
    m.add(new Circle(1));
    m.sortByArea();
    check("sort puts smallest first", m.get(0)->area(), PI);
    m.remove(0);
    check("remove works", m.size(), 1);

    cout << "Tests: " << passed << " passed, " << failed << " failed" << endl << endl;
}

// ---------------------------------------------------------------
int main() {
    runTests();

    ShapeManager manager;
    manager.add(new Circle(5));
    manager.add(new Rectangle(8, 4));
    manager.add(new Polygon({{0, 0}, {8, 0}, {4, 6}}));   // a triangle

    cout << "Before sorting:" << endl;
    manager.printAll();

    manager.sortByArea();
    cout << endl << "After sorting by area:" << endl;
    manager.printAll();
    cout << endl;

    for (int i = 0; i < manager.size(); i++) {
        showDetails(manager.get(i));
    }
    return 0;
}
