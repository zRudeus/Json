//
// Created by rzakr on 10.12.2025.
//

#ifndef JSONPARSER_JSON_H
#define JSONPARSER_JSON_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <typeinfo>

template <typename T, typename... Types>
concept OneOf = (std::is_same_v<T, Types> || ...);

class Json {
public:
    class Null{};
    using Bool = bool;
    using Number = double;
    using String = std::string;
    using Array = std::vector<Json>;
    using Object = std::unordered_map<String, Json>;

    class Visitor {
    public:
        virtual void operator()(Null&) = 0;
        virtual void operator()(Bool&) = 0;
        virtual void operator()(Number&) = 0;
        virtual void operator()(String&) = 0;
        virtual void operator()(Array&) = 0;
        virtual void operator()(Object&) = 0;
        virtual ~Visitor() = default;
    };
private:
    struct Concept{
        virtual std::unique_ptr<Concept> clone() = 0;
        virtual void visit(Visitor&) = 0;
        virtual ~Concept() = default;
    };
    template<typename T>
    requires OneOf<T, Number, Object, String, Array, Bool, Null>
    struct Model: Concept {
        Model(T&& _data): data(std::move(_data)){}
        Model(const T& _data): data(_data){}
        T data;
        std::unique_ptr<Concept> clone() override {
            return std::make_unique<Model<T>>(data);
        }
        void visit(Visitor& visitor) override {
            visitor(data);
        }
    };

    std::unique_ptr<Concept> value;
public:
    template<typename T>
    requires OneOf<T, Number, Object, String, Array, Bool, Null>
    Json(const T& value): value(std::make_unique<Model<T>>(value)){}
    template<typename T>
    requires OneOf<T, Number, Object, String, Array, Bool, Null>
    Json(T&& value): value(std::make_unique<Model<T>>(std::move(value))){}

    Json(Json&& other): value(std::move(other.value)){}
    Json(const Json& other): value(other.value->clone()){}
    Json(): Json(Null{}) {}
    Json& operator=(Json&& other) {
        value = std::move(other.value);
        return *this;
    }
    Json& operator=(const Json& other) {
        value = other.value->clone();
        return *this;
    }
    Json& operator[](std::size_t index) {
        auto& t = as<Array>();
        return t.at(index);
    }
    const Json& operator[](std::size_t index) const {
        auto& t = as<Array>();
        return t.at(index);
    }
    Json& operator()(const String& key) {
        auto& t = as<Object>();
        return t.at(key);
    }
    const Json& operator()(const String& key) const {
        auto& t = as<Object>();
        return t.at(key);
    }

    template<typename T>
    requires OneOf<T, Number, Object, String, Array, Bool, Null>
    const T& as() const {
        if (not is<T>()) throw std::bad_cast();
        return dynamic_cast<Model<T>*>(value.get())->data;
    };
    template<typename T>
    requires OneOf<T, Number, Object, String, Array, Bool, Null>
    T& as() {
        if (not is<T>()) throw std::bad_cast();
        return dynamic_cast<Model<T>*>(value.get())->data;
    };

    template<typename T>
    requires OneOf<T, Number, Object, String, Array, Bool, Null>
    bool is() const noexcept {
        return dynamic_cast<Model<T>*>(value.get());
    };

    void visit(Visitor& visitor) const {
        value->visit(visitor);
    }
};

namespace JsonDecls{
    using Null = Json::Null;
    using Bool = Json::Bool;
    using Number = Json::Number;
    using String = Json::String;
    using Array = Json::Array;
    using Object = Json::Object;
    using Visitor = Json::Visitor;
}


#endif //JSONPARSER_JSON_H
