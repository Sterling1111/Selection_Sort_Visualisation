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
Time longDelay = milliseconds(500);

namespace sound {
#define TWOPI 6.283185307

    short SineWave(double time, double freq, double amp) {
        short result;
        double tpc = 44100 / freq; // ticks per cycle
        double cycles = time / tpc;
        double rad = TWOPI * cycles;
        short amplitude = 32767 * amp;
        result = amplitude * sin(rad);
        return result;
    }
}

void render(RenderWindow* window, const std::vector<RectangleShape> r) {
    window->clear();
    for(const auto& elem : r) {
        window->draw(elem);
    }
    window->display();
}

void selectionSortThread(RenderWindow* window, bool& running, std::vector<int>& v, std::vector<RectangleShape> r, std::vector<sf::Sound>& s) {
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
        sleep(longDelay);
        r[i].setFillColor(Color::Green);
        //s[0].play();
        if(i != 0) {
            r[i-1].setFillColor(Color::White);
        }
        render(window, r);
        s[i].play();
        sleep(longDelay);
    }
    r[r.size() - 2].setFillColor(Color::White);
    r[r.size() - 1].setFillColor(Color::Green);
    render(window, r);
    r[r.size()-1].setFillColor(Color::White);
    render(window, r);
    //sleep(delayTime);
    window->setActive(false);
}

int main() {
    RenderWindow window(VideoMode(1000, 600), "Sort Visualiser");
    //window.setFramerateLimit(60);

    sf::SoundBuffer buffer;
    std::vector<sf::SoundBuffer> buffers(100);
    std::vector<sf::Sound> sounds(100);
    std::vector<sf::Int16> samples(100);
    std::vector<std::vector<sf::Int16>> vec_of_samples(100);

    for (int j = 99; j >= 0; --j) {
        for (int i = 0; i < 44100; i++) {
            samples.push_back(sound::SineWave(i, 200 + j * 10, 1));
        }
        vec_of_samples[j] = samples;
        buffers[j].loadFromSamples(&(vec_of_samples[j])[0], vec_of_samples[j].size(), 1, 44100);
        sounds[j].setBuffer(buffers[j]);
        samples.clear();
    }

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
    Thread renderThread([capture0 = &window, &running, &v, &r, &sounds] {
        return selectionSortThread(capture0, running, v, r, sounds);
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
