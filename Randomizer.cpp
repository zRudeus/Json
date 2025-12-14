//
// Created by rzakr on 14.12.2025.
//

#include "Randomizer.h"
#include <random>
using namespace std::literals::string_literals;

using namespace JsonDecls;

class Randomizer: public Visitor{
protected:
    std::mt19937 gen{std::random_device()()};
    std::uniform_real_distribution<Number> numRand{0., 100.};
    std::uniform_int_distribution<std::size_t> sizeRand{0, 20};
    std::bernoulli_distribution boolRand;
    std::string_view chars = " abcdefghij.,0123456789";
    std::uniform_int_distribution<std::size_t> charRand{0, chars.length() - 1};
public:
    void operator()(Null& val) override {}
    void operator()(Bool& val) override {
        val = boolRand(gen);
    }
    void operator()(Number& val) override {
        val = numRand(gen);
    }
    void operator()(String& val) override {
        val.resize(sizeRand(gen));
        for (auto& c: val)
            c = chars[charRand(gen)];
    }
    void operator()(Array& val) override {
        for (auto& t : val)
            t.visit(*this);
    }
    void operator()(Object& val) override {
        for (auto& [_, t] : val)
            t.visit(*this);
    }
};

void randomise(Json& payload) {
    Randomizer randomizer;
    payload.visit(randomizer);
}
Json randomise(const Json& payload) {
    auto res = payload;
    randomise(res);
    return res;
}

class Creator{
    std::mt19937 gen{std::random_device()()};
    std::uniform_int_distribution<std::size_t> typeRand{0, 5};
    std::uniform_int_distribution<std::size_t> prymRand{0, 3};
    std::uniform_int_distribution<std::size_t> sizeRand{0, 5};

    std::string_view chars = "abcdefghij_";
    std::uniform_int_distribution<std::size_t> charRand{0, chars.length() - 1};
public:
    std::string get_key(){
        std::string res;
        res.resize(sizeRand(gen) + 2);
        for (auto& c: res)
            c = chars[charRand(gen)];
        return res;
    }
    Json generate(std::size_t depth){
        switch ((depth == 0) ? prymRand(gen) : typeRand(gen)){
            case 1: return Bool{};
            case 2: return Number{};
            case 3: return String{};
            case 4: {
                Array res(sizeRand(gen));
                for (auto& elem: res) elem = generate(depth - 1);
                return res;
            }
            case 5: {
                Object res;
                std::size_t size = sizeRand(gen);
                for (std::size_t i = 0; i < size; i++){
                    res.emplace(get_key(), generate(depth - 1));
                }
                return res;
            }
            default: return {};
        }
    }
};
Json createRandom() {
    return randomise(Creator().generate(5));
}
