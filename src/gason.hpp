/**
 * @file gason.hpp
 * a simple and fast JSon parser in plain C/C++ with no dependency.
 *
 *
 * @author Ivan Vashchaev
 * @version 1.0.0
 * @date 2014-05-08
 * based on this commit: 9e292d4
 *
 * @author amir zamani
 * @version 2.1.0
 * @date 2014-05-16
 *
 */

#ifndef __GASON_HPP__
#define __GASON_HPP__
///////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
///////////////////////////////////////////////////////////////////////////////
namespace gason {
///////////////////////////////////////////////////////////////////////////////
#ifndef nullptr
#   define nullptr      NULL
#endif
///////////////////////////////////////////////////////////////////////////////
/** tag (type) of each JSon element. */
enum JsonTag {
    JSON_TAG_NUMBER = 0,        ///< double (floating point) value
    JSON_TAG_STRING,            ///< string value
    JSON_TAG_BOOL,              ///< boolean (true/false) value
    JSON_TAG_ARRAY,             ///< an array value
    JSON_TAG_OBJECT,            ///< an object value
    JSON_TAG_NULL = 0xF         ///< null or invalid value
};
///////////////////////////////////////////////////////////////////////////////
struct JsonNode;
///////////////////////////////////////////////////////////////////////////////
/** JSon value of @sa JsonTag type. */
struct JsonValue {
    union {
        uint64_t    ival;
        double      fval;
    };

    JsonValue() : ival(JSON_VALUE_NULL) {
    }
    JsonValue(double x) : fval(x) {
    }
    JsonValue(JsonTag tag, void *p) {
        uint64_t x = (uint64_t)p;
        assert(tag <= JSON_VALUE_TAG_MASK);
        assert(x <= JSON_VALUE_PAYLOAD_MASK);
        ival = JSON_VALUE_NAN_MASK | ((uint64_t)tag << JSON_VALUE_TAG_SHIFT) | x;
    }

    bool        isNumber() const {
        return getTag() == JSON_TAG_NUMBER;
    }
    bool        isBoolean() const {
        return getTag() == JSON_TAG_BOOL;
    }
    bool        isString() const {
        return getTag() == JSON_TAG_STRING;
    }
    bool        isNode() const {
        return getTag() == JSON_TAG_ARRAY ||  getTag() == JSON_TAG_OBJECT;
    }
    bool        isArray() const {
        return getTag() == JSON_TAG_ARRAY;
    }
    bool        isObject() const {
        return getTag() == JSON_TAG_OBJECT;
    }
    bool        isDouble() const {
        return (int64_t)ival <= (int64_t)JSON_VALUE_NAN_MASK;
    }
    JsonTag     getTag() const {
        return isDouble() ? JSON_TAG_NUMBER : JsonTag((ival >> JSON_VALUE_TAG_SHIFT) & JSON_VALUE_TAG_MASK);
    }

    int         toInt(bool* ok = nullptr) const {
        return (int) toNumber(ok);
    }
    double      toNumber(bool* ok = nullptr) const {
        if ( !checkType(isNumber(), ok) )
            return 0.0;

        return fval;
    }
    bool        toBool(bool* ok = nullptr) const {
        if ( !checkType(isBoolean(), ok) )
            return false;

        return (bool)getPayload();
    }
    char*       toString(bool* ok = nullptr) const {
        if ( !checkType(isString(), ok) )
            return nullptr;

        return (char *)getPayload();
    }
    JsonNode*   toNode(bool* ok = nullptr) const {
        if ( !checkType(isNode(), ok) )
            return nullptr;

        return (JsonNode *)getPayload();
    }

    /** returns true if this object is not NULL. */
    operator bool()const {
        return getTag() != JSON_TAG_NULL;
    }
    /** returns true if this object has typeof tag value. */
    bool        operator==(JsonTag tag) const {
        return getTag() == tag;
    }
    /** returns true if this object is not typeof tag value. */
    bool        operator!=(JsonTag tag) const {
        return getTag() != tag;
    }

    /** overloads @sa at. */
    JsonValue   operator[](size_t index) const {
        return at(index);
    }
    /** overloads @sa child. */
    JsonValue   operator()(const char* keyName) const {
        return child(keyName);
    }
    /** returns a child value associated with the key = keyName. */
    JsonValue   child(const char* keyName) const;
    /** returns the item at index position i in the array. */
    JsonValue   at(size_t i) const;

protected:
    uint64_t    getPayload() const {
        assert(!isDouble());
        return ival & JSON_VALUE_PAYLOAD_MASK;
    }

    bool        checkType(bool isMatched, bool *ok) const {
        if ( ok == 0 ) {
            assert(isMatched && "failed to convert a JsonValue because of type difference.");
            return true;
        }

        *ok = isMatched;
        return isMatched;
    }

    static const uint64_t JSON_VALUE_PAYLOAD_MASK = 0x00007FFFFFFFFFFFULL;
    static const uint64_t JSON_VALUE_NAN_MASK     = 0x7FF8000000000000ULL;
    static const uint64_t JSON_VALUE_NULL         = 0x7FFF800000000000ULL;
    static const uint64_t JSON_VALUE_TAG_MASK     = 0xF;
    static const uint64_t JSON_VALUE_TAG_SHIFT    = 47;
};
///////////////////////////////////////////////////////////////////////////////
struct JsonNode {
    JsonValue   value;
    JsonNode*   next;
    char*       key;
};
///////////////////////////////////////////////////////////////////////////////
struct JsonIterator {
    JsonNode*   p;

    explicit JsonIterator(JsonNode* n = nullptr) : p(n) {
    }

    void       operator++() {
        p = p->next;
    }
    void       operator++(int) {
        p = p->next;
    }

    bool       isValid()const {
        return p != nullptr;
    }
    bool       hasNext()const {
        return p->next != nullptr;
    }

    bool       operator==(const char* key) const {
        return strncmp(p->key, key, strlen(key)) == 0;
    }
    bool       operator!=(const JsonIterator &x) const {
        return p != x.p;
    }

    JsonNode*  operator*() const {
        return p;
    }
    JsonNode*  operator->() const {
        return p;
    }
};

inline JsonIterator begin(JsonValue o) {
    return JsonIterator(o.toNode());
}
inline JsonIterator end(JsonValue) {
    return JsonIterator(nullptr);
}
///////////////////////////////////////////////////////////////////////////////

enum JsonParseStatus {
    JSON_PARSE_OK,
    JSON_PARSE_BAD_NUMBER,
    JSON_PARSE_BAD_STRING,
    JSON_PARSE_BAD_IDENTIFIER,
    JSON_PARSE_STACK_OVERFLOW,
    JSON_PARSE_STACK_UNDERFLOW,
    JSON_PARSE_MISMATCH_BRACKET,
    JSON_PARSE_UNEXPECTED_CHARACTER,
    JSON_PARSE_UNQUOTED_KEY,
    JSON_PARSE_BREAKING_BAD
};

/** Automatic memory manager for parsed JsonValue.
 * @sa jsonPars().
 * used internally by jsonParse(). you do not need to manually handle this object.
 * just keep allocator instance as long as you want to keep JsonValues.
 */
class JsonAllocator {
public:
    JsonAllocator() : head(nullptr) {
    }
    ~JsonAllocator() {
        deallocate();
    }

    /** frees the allocated memory.
     * you do not need to call this function. If for some reason you have to
     *  release the memory manually, beware that after deallocate(),
     *  all the parsed JsonValues are invalid.
     */
    void    deallocate();

protected:
    struct  Zone;
    Zone*   head;

    void*   allocate(size_t size);
    void    reset();

    friend JsonParseStatus jsonParse(char*, char**, JsonValue*, JsonAllocator&);
};

JsonParseStatus
jsonParse(char *str, char **endptr, JsonValue *value, JsonAllocator &allocator);

/** parses a JSon string and return root object or array.
 * JsonValues are the index tree for actual data on jsonString. allocator object holds
 *  and manage these indices.
 *
 * to keep the JsonValue valid, you have to keep both jsonString (data)
 *  and allocator (instances) alive.
 *
 * to avoid calling alloc/free many times (a serious problem on embedded devices)
 *  the allocator is re-usable and you can hold and re-use instance for parsing
 *  as many as jsonStrings you want. JsonAllocator automatically expands
 *  if more memory is required and frees all memories automatically.
 *
 * @param jsonString the JSon string. this buffer will be modified by this function.
 * @param root root object of JSon string. could be an object or array.
 * @param allocator an instance to memory manager.
 *  this function calls allocator.reset() before parsing.
 * @return returns JSON_PARSE_OK or a proper error code.
 */
inline JsonParseStatus
jsonParse(char* jsonString, JsonValue& root, JsonAllocator& allocator) {
    char *endptr = 0;
    return jsonParse(jsonString, &endptr, &root, allocator);
}
///////////////////////////////////////////////////////////////////////////////
} // namespace gason
///////////////////////////////////////////////////////////////////////////////
#endif // __GASON_HPP__
