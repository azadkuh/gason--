# gason++

gason++ is a light tweak for [gason](https://github.com/vivkin/gason) parser to make its usage even simpler for end-users.
There is also a simple JSon builder tool.

gason is an **efficient** and fantastic JSon parser in plain C++ with minimum dependency by [vivkin](https://github.com/vivkin).

please read gason documents to see the full list of features and other notes.

## Table of Contents
- [Notes](#notes)
- [Installation](#installation)
- [Usage](#usage)
    - [Parsing](#parsing)
    - [Child elements](#child-elements)
    - [Type checking](#type-checking)
    - [Conversion](#conversion)
    - [Iteration](#iteration)
    - [Building](#building)
- [License](#license)

## Notes
[TOC](#table-of-contents)

gason(gason++) is efficient and fast and it does not consume any extra memory for parsed values (objects, arrays, ...)

* gason(gason++) is a **destructive** parser:
> your **source buffer** will be **modified**! and this buffer is the only place where the values do exist.

* there is a simple and efficient `JSonBuilder` which is not present on original `gason`.

* `c++11` is optional and `gason++` is also compilable on older toolchains. `gason` requires `c++11`.


## Installation
[TOC](#table-of-contents)

Download latest version and just add [gason.hpp](./src/gason.hpp) / [gason.cpp](./src/gason.cpp) and [jsonbuilder.hpp](./src/jsonbuilder.hpp) from [src](./src) folder into your project tree.
*Other `*.pro` and source files are just for testing purpose.*



## Usage
[TOC](#table-of-contents)

suppose this json:
```json
{
  "an_array": [
    10,
    11,
    12
  ],
  "a_boolean": true,
  "a_null": null,
  "a_number": 123,
  "an_object": {
    "a": "b",
    "c": "d",
    "e": "f"
  },
  "a_message": "Hello World"
}
```

### Parsing
[TOC](#table-of-contents)

to parse it simply do:
```cpp
// this buffer holds the json content.
// char *jsonString;

gason::JsonAllocator    allocator;
gason::JsonValue        root;
gason::JsonParseStatus  status = gason::jsonParse(jsonString, root, allocator);

if ( status != gason::JSON_PARSE_OK ) {
    puts("parsing failed!");
    // for more info check the status.
    return false;
}

// hint: jsonString will be over-written by jsonParse()
//        and you have to keep this buffer and allocator alive
//        as long as the parsed JSon values are required.
```

see [sample:parser](./tests/parser1/main.cpp) for more examples.

### Child elements
[TOC](#table-of-contents)

gason++ introduces an easy API to retrieve the child elements:

```cpp
gason::JsonValue str  = root.child("a_message");        // = Hello World
gason::JsonValue arr0 = root.child("an_array").at(0);   // = 10
// short form
gason::JsonValue arr2 = root("an_array")[2];            // = 12
gason::JsonValue objc = root("an_object")("c");         // = d
```
> All **values** will become **invalid** when **allocator** be **destroyed**.


### Type checking
[TOC](#table-of-contents)

to check validity or the type of values:
```cpp
if ( !str ) {
    puts("str is not a valid JsonValue!");
}
if ( arr2    &&    arr2.isNumber() ) {
    puts("a valid number has been found on an_array[2].");
}
if ( objc    &&    objc.isString() ) {
    puts("an string has been found in root->an_object->c.");
}
```

### Conversion
[TOC](#table-of-contents)

`JsonValue` has some `toXXX(bool *ok = nullptr)` to convert a value into an int, string, ...

if conversion fails, the conversion methods:

* fire `assert()` if `ok` is not specified.
* return a `0`,`nullptr` or `false` value and `*ok` will be set to `false`.

```cpp
bool ok = false;

int  invalidValue1 = str.toInt();   // will assert()

const char* invalidValue2 = arr2.toString(&ok); // won't assert()
if ( !ok ) {
    puts("arr2 is an int not a string!");
}
```

### Iteration
[TOC](#table-of-contents)

to iterate over children (elements) simply:
```cpp
gason::JsonValue obj = root("an_object");

for ( gason::JsonIterator it =  gason::begin(obj); gason::end(obj); it++ ) {
    printf("%s = %s\n", it->key, it->value.toString());
}

// or
gason::JsonIterator it = gason::begin( obj );
while ( it.isValid() ) {
    printf("%s = %s\n", it->key, it->value.toString());

    it++;
}

// both prints:
// a = b
// c = d
// e = f
```

### Building
[TOC](#table-of-contents)

to build the above json:
```cpp
char buffer[257] = {0};
gason::JSonBuilder doc(buffer, 256);

doc.startObject()
        .startArray("an_array")
            .addValue(10)
            .addValue(11)
            .addValue(12)
        .endArray()

        .addValue("a_boolean", true)
        .addNull("a_null")
        .addValue("a_number", 123)

        .startObject("an_object")
            .addValue("a", "b")
            .addValue("c", "d")
            .addValue("e", "f")
        .endObject()

        .addValue("a_message", "Hello World")

        .endObject();

// now the buffer contains the proper json string.

if ( !doc.isBufferAdequate() ) {
    puts("warning: the buffer is small and the output json is not valid.");
}
```

> JSonBuilder just makes **compact** form of JSon strings suitable for storage or network communications. (the output is not indented.)

see [sample:builder](./tests/builder1/main.cpp) for more examples.


## License
[TOC](#table-of-contents)

Distributed under the MIT license. Copyright (c) 2014, Amir Zamani.
