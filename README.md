# gason++

gason++ is a light tweak for [gason](https://github.com/vivkin/gason) parser to make its usage even simpler for end-users.
There is also a simple JSon builder tool.

gason is an **efficient** and fantastic JSon parser in plain C++ with minimum dependency by [vivkin](https://github.com/vivkin).

please read gason documents to see the full list of features and other notes.

## TOC
- [Notes](#Notes)
- [Installation](#installation)
- [Usage](#usage)
    - [Parsing](#parsing)
    - [Iteration](#iteration)
    - [Building](#building)
- [License](#license)

## Notes
* gason(gason++) is efficient and fast and it does not consume any extra memory for parsed values (objects, arrays, ...)

* gason(gason++) is a **destructive** parser:
> your **source buffer** will be **modified**! and this buffer is the only place where the values do exist.

* there is a simple and efficient JSon builder which is not present on original gason.


## Installation
1. Download latest version and just add `gason.hpp` / `gason.cpp` and `jsonbuilder.hpp` from **src** folder to your project tree. other `*.pro` and source files are just for testin purpose.
2. Compile with C++11 support (`-std=c++11` flag for gcc/clang). next version will be compilable without c++11.


## Usage
suppose this json:
```json
{
  "array": [
    0,
    1,
    2
  ],
  "boolean": true,
  "null": null,
  "number": 123,
  "object": {
    "a": "b",
    "c": "d",
    "e": "f"
  },
  "string": "Hello World"
}
```

### Parsing
to parse it simply do:
```cpp
char buffer[257] = {0};
// copy json content into buffer

gason::JsonAllocator    allocator;
gason::JsonValue        root;

////////////////////////////
// parse
////////////////////////////
// -> buffer will be over-written by jsonParse() <-
if ( gason::jsonParse(buffer, root, allocator) != gason::JSON_PARSE_OK ) {
    puts("parsing failed!");
    return false;
}

////////////////////////////
// read / find / checks
////////////////////////////
// reads value:          root.array[1] = 1
int array1 = (int) root.child("array").at(1).toNumber();
int array2 = (int) root("array")[2].toNumber();  // short form

// checks type:          root.number = 123 is a number type (tag)
bool bnumber  = root("number") == gason::JSON_TAG_NUMBER;

// prints child object: root.object.c = d
puts( root("object")("c").toString() ); // prints d, short form
puts( root.child("object")
          .child("e").toString()
    ); // prints f
```
> All **values** will become **invalid** when **allocator** be **destroyed**.

### Iteration
to iterate over `object` children (elements) simply:
```cpp
gason::JsonValue childObject = root.child("object");

for ( gason::JsonIterator it =  gason::begin(childObject);
      it != gason::end(childObject);    it++) {
    printf("%s = %s\n",
           it->key, it->value.toString()
           );
    // prints:
    // a = b
    // c = d
    // e =f
}
```

### Building
to build the above json:
```cpp
char buffer[257] = {0};
gason::JSonBuilder doc(buffer, 256);

doc.startObject()
        .startArray("array")
        .addValue(0)
        .addValue(1)
        .addValue(2)
        .endArray()

   .addValue("boolean", true)
   .addNull("null")
   .addValue("number", 123)
   .startObject("object")
        .addValue("a", "b")
        .addValue("c", "d")
        .addValue("e", "f")
    .endObject()

    .addValue("string", "Hello World")
    .endObject();

// now the buffer contains the proper json string.
```

## License
Distributed under the MIT license. Copyright (c) 2014, Amir Zamani.
