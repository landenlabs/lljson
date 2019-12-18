//-------------------------------------------------------------------------------------------------
//
// File: json.h   Author: Dennis Lang  Desc: Parse json
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2019
// http://landenlabs.com
//
// This file is part of lljson project.
//
// ----- License ----
//
// Copyright (c) 2019 Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef json_h
#define json_h

#include <vector>
#include <map>
#include <algorithm>
#include <regex>
#include <exception>
using namespace std;

typedef std::vector<lstring> StringList;
typedef std::map<string, StringList> MapList;

// Base class for all Json objects
class JsonBase {
public:
    enum Jtype { None, Value, Array, Map };
    Jtype mJtype = None;
    JsonBase(Jtype jtype) {
        mJtype = jtype;
    }
    JsonBase(const JsonBase& other) {
          mJtype = other.mJtype;
    }
    virtual
    string toString() const = 0;
    
    virtual
    ostream& dump(ostream& out) const = 0;
    
    virtual
    void toMapList(MapList& mapList, string key) const = 0;
};


// Simple Value
class JsonValue : public JsonBase, public string {
public:
    const char* quote = "\"";
    
    bool isQuoted = false;
    JsonValue() : JsonBase(Value), string() {
    }
    JsonValue(const char* str) : JsonBase(Value), string(str) {
    }
    JsonValue(string& str) : JsonBase(Value), string(str) {
    }
    JsonValue(const JsonValue& other) : JsonBase(other), string(other), isQuoted(other.isQuoted) {
        // cerr << isQuoted;
    }
    void clear() {
        isQuoted = false;
        string::clear();
    }
    ostream& dump(ostream& out) const {
        out << toString();
        return out;
    }
    
    void toMapList(MapList& mapList, string key) const {
        cerr << toString();
    }
    
    string toString() const {
        if (isQuoted) {
            return string(quote) + *this + string(quote);
        }
        return *this; // ->c_str();
    }
};

typedef std::vector<JsonBase*> VecJson;
typedef std::map<JsonValue, JsonBase*> MapJson;


// Array of Json objects
class JsonArray : public JsonBase, public VecJson {
public:
    JsonArray() : JsonBase(Array) {
    }
    
    string toString() const {
        std::ostringstream ostr;
        ostr << "[\n";
        JsonArray::const_iterator it = begin();
        bool addComma = false;
        while (it != end())
        {
            if (addComma)
                ostr << ",\n";
            addComma = true;
            ostr << (*it++)->toString();
        }
        ostr << "\n]";
        return ostr.str();
    }
    
    ostream& dump(ostream& out) const {
        cout << toString();
        return cout;
    }
    
    void toMapList(MapList& mapList, string key) const {
        StringList& list = mapList[key];
        JsonArray::const_iterator it = begin();
        while (it != end())
        {
            list.push_back((*it++)->toString());
        }
    }
};

// Map (group) of Json objects
class JsonMap : public JsonBase, public MapJson {
public:
    JsonMap() : JsonBase(Map), MapJson() {
    }
    
    string toString() const {
        ostringstream out;
        out << "{\n";
        JsonMap::const_iterator it = begin();
        bool addComma = false;
        while (it != end())
        {
            if (addComma)
                out << ",\n";
            addComma = true;
            
            JsonValue name = it->first;
            JsonBase* pValue = it->second;
            out << name.toString() << ": " << pValue->toString();
            it++;
        }
        out << "\n}\n";
        return out.str();
    }
    
    ostream& dump(ostream& out) const {
        out << toString();
        return out;
    }
    
    void toMapList(MapList& mapList, string key) const {
        JsonMap::const_iterator it = begin();
        while (it != end())
        {
            std::string name = it->first;
            JsonBase* pValue = it->second;
            pValue->toMapList(mapList, name);
            it++;
        }
    }
};

// Alternate name JsonFields for JsonMap
typedef JsonMap JsonFields;

// String buffer being parsed
class JsonBuffer : public std::vector<char> {
public:
    char keyBuf[10];
    
    size_t pos = 0;
    int seq = 100;
    char nextChr() {
        if (pos < size()) {
            return at(pos++);
        }
        return NULL;
    }
    
    string nextKey() {
        snprintf(keyBuf, sizeof(keyBuf), "%03d", seq++);
        return *(new string(keyBuf));
    }
    
    const char* ptr(int len = 0) {
        const char* nowPtr = &at(pos);
        pos = std::min(pos + len, size());
        return nowPtr;
    }
};

// Json value or parse state change.
class JsonToken : public JsonValue {
public:
    enum Token { Value, EndArray, EndGroup, EndParse } ;
    Token mToken = Value;

    JsonToken() : JsonValue() {
    }
    JsonToken(const char* str) : JsonValue(str) {
    }
    JsonToken(Token token) : JsonValue(), mToken(token) {
    }
    JsonToken(const JsonToken& other) : JsonValue(other), mToken(other.mToken) {
    }
};

static JsonToken END_ARRAY(JsonToken::EndArray);
static JsonToken END_GROUP(JsonToken::EndGroup);
static JsonToken END_PARSE(JsonToken::EndParse);


#endif /* json_h */
