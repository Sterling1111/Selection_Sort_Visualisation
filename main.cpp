#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>

using namespace sf;

Time delayTime = milliseconds(5);
Time shortDelay = microseconds(50);


void render(RenderWindow* window, const std::vector<RectangleShape> r) {
    window->clear();
    for(const auto& elem : r) {
        window->draw(elem);
    }
    window->display();
}

void selectionSortThread(RenderWindow* window, bool& running, std::vector<int>& v, std::vector<RectangleShape> r) {
    render(window, r);
    sleep(delayTime);
    size_t size{v.size()};
    for(unsigned i{0}; i < size - 1 && running; i++) {
        unsigned minIndex{i};
        r[minIndex].setFillColor(Color::Red);
        render(window, r);
        sleep(delayTime);
        for(unsigned j{i+1}; j < size && running; j++) {
            r[j].setFillColor(Color(255, 255, 255, 50));
            //r[j].setFillColor(Color::Red);
            render(window, r);
            sleep(shortDelay);
            r[j].setFillColor(Color::White);
            if(v[j] < v[minIndex]) {
                r[minIndex].setFillColor(Color::White);
                minIndex = j;
                r[minIndex].setFillColor(Color::Red);
                render(window, r);
                sleep(delayTime);
            }
        }
        std::swap(v[i], v[minIndex]);
        auto tempPositionX = r[i].getPosition().x;
        r[i].setPosition(Vector2f(r[minIndex].getPosition().x, r[i].getPosition().y));
        r[minIndex].setPosition(Vector2f(tempPositionX, r[minIndex].getPosition().y));
        std::swap(r[i], r[minIndex]);
        r[i].setFillColor(Color::Green);
        if(i != 0) {
            r[i-1].setFillColor(Color::White);
        }
        render(window, r);
        //sleep(delayTime);
    }
    r[r.size() - 2].setFillColor(Color::White);
    r[r.size() - 1].setFillColor(Color::Green);
    render(window, r);
    sleep(delayTime);
    r[r.size()-1].setFillColor(Color::White);
    render(window, r);
    sleep(delayTime);
    window->setActive(false);
}

int main() {
    RenderWindow window(VideoMode(1000, 600), "Sort Visualiser");
    //window.setFramerateLimit(60);

    std::vector<int> v(100);
    std::vector<RectangleShape> r(100);
    for (int i = 0; i < 100; ++i) {
        v[i] = i + 1;
    }
    std::shuffle(v.begin(), v.end(), std::mt19937(std::random_device()()));
    for (int i = 0; i < v.size(); ++i) {
        r[i] = RectangleShape(Vector2f(window.getSize().x / 100 - 1, v[i] * (window.getSize().y / 100)));
        r[i].setPosition(i * 10, window.getSize().y - r[i].getSize().y);
        r[i].setOutlineColor(Color::Black);
    }
    bool running{true};
    Thread renderThread([capture0 = &window, &running, &v, &r] {
        return selectionSortThread(capture0, running, v, r);
    });
    window.setActive(false);
    renderThread.launch();

    Time timeDelay2 = milliseconds(10);

    while(running) {
        sleep(timeDelay2);
        Event event;
        while(window.pollEvent(event)) {
            if(event.type == Event::Closed) {
                running = false;
            }
        }
    }
    renderThread.wait();
    window.setActive(true);
    window.close();

    return 0;
}
