# Type-Erased JSON for C++ 23

A modern C++ JSON library built around **explicit type erasure** and a **visitor-based dispatch model**.
The core `Json` type provides a single, value-semantic handle for all JSON data while keeping
type-specific logic fully encapsulated.

The library is designed to be minimal, predictable.

---

## Examples
~~~c++
#include "Json.h"       // Base class
#include "Printing.h"   // Support for std::ostream
#include "Parser.h"     // Basic parsing utility
#include "Randomizer.h" // Basic field randomization 

using namespace JsonDecls;
~~~
~~~c++
Json obj = Object(
    { { "key", "Some array" },
      { "value", Array({1., 2., 3.})}
    });
obj("value")[1] = "Some new value";
std::cout << obj("value")[1].is<String>() << std::endl;
~~~
~~~c++
std::ifstream input("dump.json");
std::ofstream output("res.txt");
Json res = parse(input);
output << res;
~~~
~~~c++
Json t = createRandom();
std::cout << t;
~~~

---

## Design

* Manual type erasure for full control
* No `std::variant` or RTTI-based unions
* Visitor pattern for extensibility
* Strong value semantics
* Explicit runtime checks over implicit conversions

---

## Core Types: `Json`

* `Json` is a value type that owns exactly one JSON value at runtime.
  * A type-erased `Concept` interface
  * A templated `Model<T>` implementation per JSON type
  * Deep-copy semantics via virtual `clone()`
  * All copies of `Json` are independent and safe.
* `Json::Null` – empty tag type representing `null`
* `Json::Bool` – `bool`
* `Json::Number` – `double`
* `Json::String` – `std::string`
* `Json::Array` – `std::vector<Json>`
* `Json::Object` – `std::unordered_map<String, Json>`

Construction is restricted to these types at compile time.

---

## Inspection and Access

The API provides **explicit runtime type checks**:

* `is<T>()`
  Returns `true` if the stored value is of type `T`
* `as<T>()`
  Returns a mutable reference to the stored value
* `operator[]`
  Access array elements (bounds-checked)
* `operator()`
  Access object members by key
Access functions assume the underlying type is correct and will throw `std::bad_cast` if misused.

---

## Construction and Assignment

`Json` supports:

* Default construction (`null`)
* Construction from any valid JSON type
* Copy construction (deep copy)
* Move construction
* Copy assignment (deep copy)
* Move assignment

All operations preserve value semantics.

---

## Visitor Interface

The library uses a **non-templated visitor** for runtime dispatch.

### `Json::Visitor`

The visitor defines one overload per JSON type:

* `Null`
* `Bool`
* `Number`
* `String`
* `Array`
* `Object`

Visitation is performed via:

* `Json::visit(Visitor&)`

This design allows:

* External operations without modifying `Json` class
* Clear separation of algorithms (printing, parsing, validation)
* Stable ABI for extension libraries

---

## License

MIT License.
See `LICENSE` for details.
