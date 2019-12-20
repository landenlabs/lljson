//-------------------------------------------------------------------------------------------------
//
//  lljson      Dec-2019       Dennis Lang
//
//  Convert json to csv (tanspose arrays into columns)
//
// Example input json:
//  {
//     "cloudCover": [
//       10,
//       30,
//       49
//     ],
//       "dayOfWeek": [
//       "Monday",
//       "Tuesday",
//       "Wednesday"
//     ]
//  }
//
//  Output trasnposed CSV
//   cloudCover,  dayOfWeek
//    10, Monday
//    30, Tuesday
//    49, WednesDay
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2019
// http://landenlabs.com/
//
// This file is part of lljson project.
//
// ----- License ----
//
// Copyright (c) 2016 Dennis Lang
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

// 4291 - No matching operator delete found
#pragma warning( disable : 4291 )

#include <stdio.h>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <regex>
#include <exception>

// Project files
#include "ll_stdhdr.h"
#include "directory.h"
#include "split.h"
#include "json.h"

using namespace std;

// Helper types
typedef std::vector<std::regex> PatternList;
typedef unsigned int uint;


// Runtime options
PatternList includeFilePatList;
PatternList excludeFilePatList;
StringList fileDirList;
bool showFile = true;
bool verbose = false;

uint optionErrCnt = 0;
uint patternErrCnt = 0;


#ifdef WIN32
const char SLASH_CHAR('\\');
#else
const char SLASH_CHAR('/');
#endif

// ---------------------------------------------------------------------------
// Extract name part from path.
lstring& getName(lstring& outName, const lstring& inPath)
{
    size_t nameStart = inPath.rfind(SLASH_CHAR) + 1;
    if (nameStart == 0)
        outName = inPath;
    else
        outName = inPath.substr(nameStart);
    return outName;
}

// ---------------------------------------------------------------------------
// Return true if inName matches pattern in patternList
bool FileMatches(const lstring& inName, const PatternList& patternList, bool emptyResult)
{
    if (patternList.empty() || inName.empty())
        return emptyResult;
    
    for (size_t idx = 0; idx != patternList.size(); idx++)
        if (std::regex_match(inName.begin(), inName.end(), patternList[idx]))
            return true;
    
    return false;
}

// ---------------------------------------------------------------------------
// Parse json word surrounded by quotes.
static void getJsonWord( JsonBuffer& buffer, char delim, JsonToken& word) {
    
    const char* lastPtr = strchr(buffer.ptr(), delim);
    word.clear();
    int len = int(lastPtr - buffer.ptr());
    word.append(buffer.ptr(len+1), len);
    word.isQuoted = true;
}

// Forward definition
static JsonToken parseJson(JsonBuffer& buffer, JsonFields& jsonFields);

// ---------------------------------------------------------------------------
// Parse json array
static void getJsonArray(JsonBuffer& buffer, JsonArray& array) {
    JsonFields jsonFields;
    for(;;) {
        JsonToken token = parseJson(buffer, jsonFields);
        if (token.mToken == JsonToken::Value) {
            JsonValue* jsonValue = new JsonValue(token);
            array.push_back(jsonValue);
        } else {
            return;
        }
    }
}

// ---------------------------------------------------------------------------
// Parse json group
static void getJsonGroup(JsonBuffer& buffer, JsonFields& fields) {
    
    for(;;) {
        JsonToken token = parseJson(buffer, fields);
        if (token.mToken == JsonToken::EndGroup) {
            return;
        }
    }
}

// ---------------------------------------------------------------------------
static void addJsonValue(JsonFields& jsonFields, JsonToken& fieldName, JsonToken& value) {
    if (!fieldName.empty() && !value.empty()) {
        jsonFields[fieldName] = new JsonToken(value);
        fieldName.clear();
        value.clear();
    }
}

// ---------------------------------------------------------------------------
static JsonToken parseJson(JsonBuffer& buffer, JsonFields& jsonFields) {
    
    JsonToken fieldName = "";
    JsonToken fieldValue;
    JsonToken tmpValue;
    
    while (buffer.pos < buffer.size()) {
        char chr = buffer.nextChr();
        switch (chr) {
            default:
                fieldValue += chr;
                break;
                
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                addJsonValue(jsonFields, fieldName, fieldValue);
                break;
            case ',':
                tmpValue = fieldValue;
                addJsonValue(jsonFields, fieldName, fieldValue);
                return tmpValue;
                
            case ':':
                fieldName = fieldValue;
                fieldValue.clear();
                break;
                
            case '{':
            {
                JsonFields* pJsonFields = new JsonFields();
                jsonFields[fieldName] = pJsonFields;
                getJsonGroup(buffer, *pJsonFields);
            }
                break;
            case '}':
                addJsonValue(jsonFields, fieldName, fieldValue);
                return END_GROUP;
                
            case '"':
                getJsonWord(buffer, '"', fieldValue);
                break;
            case '[':
            {
                JsonArray* pJsonArray = new JsonArray();
                jsonFields[fieldName] = pJsonArray;
                getJsonArray(buffer, *pJsonArray);
            }
                break;
            case ']':
                return END_ARRAY;
        }
    }
    
    return END_PARSE;
}

// ---------------------------------------------------------------------------
// Dump parsed json in json format.
void JsonDump(const JsonFields& base, ostream& out) {
    // If json parsed, first node can be ignored.
    if (base.at("") != NULL) {
        base.at("")->dump(out);
    }
}

// ---------------------------------------------------------------------------
// Output json in CSV format with the arrays as columns.
void JsonTranspose(const JsonFields& base, ostream& out) {
    if (base.at("") != NULL) {
        MapList mapList;
        StringList keys;
        base.at("")->toMapList(mapList, keys);
        
        MapList::iterator it = mapList.begin();
        bool addComma = false;
        size_t maxRows = 0;
        while (it != mapList.end()) {
            if (addComma) cout << ", ";
            addComma = true;
            
            out << it->first;
            maxRows = std::max(maxRows, it->second.size());
            it++;
        }
        out << std::endl;
        
        
        for (int row=0; row < maxRows; row++) {
            addComma = false;
            for (it = mapList.begin(); it != mapList.end(); it++) {
                if (addComma) cout << ", ";
                addComma = true;
                out << it->second.at(row);
            }
            out << std::endl;
        }
        out << std::endl;
    }
}

// ---------------------------------------------------------------------------
// Open, read and parse file.
bool ParseFile(const lstring& filepath, const lstring& filename)
{
    ifstream        in;
    ofstream        out;
    struct stat     filestat;
    JsonFields fields;
    
    try {
        if (stat(filepath, &filestat) != 0)
            return false;
        
        in.open(filepath);
        if (in.good())
        {
            JsonBuffer buffer;
            buffer.resize(filestat.st_size+1);
            streamsize inCnt = in.read(buffer.data(), buffer.size()).gcount();
            assert(inCnt < buffer.size());
            in.close();
            buffer.push_back('\0');
            
            
            parseJson(buffer, fields);
        }
        else
        {
            cerr << strerror(errno) << ", Unable to open " << filepath << endl;
        }
    }
    catch (exception ex)
    {
        cerr << ex.what() << ", Error in file:" << filepath << endl;
    }
    
    if (verbose) {
        JsonDump(fields, cout);
    } else {
        JsonTranspose(fields, cout);
    }
    
    return false;
}


// ---------------------------------------------------------------------------
// Locate matching files which are not in exclude list.
static size_t InspectFile(const lstring& fullname)
{
    size_t fileCount = 0;
    lstring name;
    getName(name, fullname);
    
    if (!name.empty()
        && !FileMatches(name, excludeFilePatList, false)
        && FileMatches(name, includeFilePatList, true))
    {
        if (ParseFile(fullname, name))
        {
            fileCount++;
            if (showFile)
                std::cout << fullname << std::endl;
        }
    }
    
    return fileCount;
}

// ---------------------------------------------------------------------------
// Recurse over directories, locate files.
static size_t InspectFiles(const lstring& dirname)
{
    Directory_files directory(dirname);
    lstring fullname;
    
    size_t fileCount = 0;
    
    struct stat filestat;
    try {
        if (stat(dirname, &filestat) == 0 && S_ISREG(filestat.st_mode))
        {
            fileCount += InspectFile(dirname);
        }
    }
    catch (exception ex)
    {
        // Probably a pattern, let directory scan do its magic.
    }
    
    while (directory.more())
    {
        directory.fullName(fullname);
        if (directory.is_directory())
        {
            fileCount += InspectFiles(fullname);
        }
        else if (fullname.length() > 0)
        {
            fileCount += InspectFile(fullname);
        }
    }
    
    return fileCount;
}

// ---------------------------------------------------------------------------
// Return compiled regular expression from text.
std::regex getRegEx(const char* value)
{
    try {
        std::string valueStr(value);
        return std::regex(valueStr);
        // return std::regex(valueStr, regex_constants::icase);
    }
    catch (const std::regex_error& regEx)
    {
        std::cerr << regEx.what() << ", Pattern=" << value << std::endl;
    }
    
    patternErrCnt++;
    return std::regex("");
}

// ---------------------------------------------------------------------------
// Validate option matchs and optionally report problem to user.
bool ValidOption(const char* validCmd, const char* possibleCmd, bool reportErr = true)
{
    // Starts with validCmd else mark error
    size_t validLen = strlen(validCmd);
    size_t possibleLen = strlen(possibleCmd);
    
    if ( strncasecmp(validCmd, possibleCmd, std::min(validLen, possibleLen)) == 0)
        return true;
    
    if (reportErr)
    {
        std::cerr << "Unknown option:'" << possibleCmd << "', expect:'" << validCmd << "'\n";
        optionErrCnt++;
    }
    return false;
}


// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{  
    if (argc == 1)
    {
        cerr << "\n" << argv[0] << "  Dennis Lang v1.1 (landenlabs.com) " __DATE__ << "\n"
        << "\nDes: Json parse and output as transposed CSV\n"
        "Use: lljson [options] directories...   or  files\n"
        "\n"
        " Options (only first unique characters required, options can be repeated):\n"
        "   -includefile=<filePattern>\n"
        "   -excludefile=<filePattern>\n"
        "   -verbose\n"
        "\n"
        " Example:\n"
        "   lljson -inc=*.json -ex=foo.json -ex=bar.json dir1/subdir dir2 file1.json file2.json "
        "\n"
        " Example input json:\n"
        "   {\n"
        "      \"cloudCover\": [\n"
        "        10,\n"
        "        30,\n"
        "        49\n"
        "      ],\n"
        "        \"dayOfWeek\": [\n"
        "        \"Monday\",\n"
        "        \"Tuesday\",\n"
        "        \"Wednesday\"\n"
        "      ]\n"
        "   }\n"
        "\n"
        "   Output transposed CSV\n"
        "    cloudCover,  dayOfWeek\n"
        "     10, Monday\n"
        "     30, Tuesday\n"
        "     49, WednesDay\n"
        "\n";
    }
    else
    {
        bool doParseCmds = true;
        string endCmds = "--";
        for (int argn = 1; argn < argc; argn++)
        {
            if (*argv[argn] == '-' && doParseCmds)
            {
                lstring argStr(argv[argn]);
                Split cmdValue(argStr, "=", 2);
                if (cmdValue.size() == 2)
                {
                    lstring cmd = cmdValue[0];
                    lstring value = cmdValue[1];
                    
                    switch (cmd[1])
                    {
                        case 'i':   // includeFile=<pat>
                            if (ValidOption("includefile", cmd+1))
                            {
                                ReplaceAll(value, "*", ".*");
                                includeFilePatList.push_back(getRegEx(value));
                            }
                            break;
                        case 'e':   // excludeFile=<pat>
                            if (ValidOption("excludefile", cmd+1))
                            {
                                ReplaceAll(value, "*", ".*");
                                excludeFilePatList.push_back(getRegEx(value));
                            }
                            break;
                            
                        default:
                            std::cerr << "Unknown command " << cmd << std::endl;
                            optionErrCnt++;
                            break;
                    }
                } else {
                    switch (argStr[1]) {
                        case 'v':   // -v=true or -v=anyThing
                            verbose = true;
                            continue;
                    }
                    
                    if (endCmds == argv[argn]) {
                        doParseCmds = false;
                    } else {
                        std::cerr << "Unknown command " << argStr << std::endl;
                        optionErrCnt++;
                    }
                }
            }
            else
            {
                // Store file directories
                fileDirList.push_back(argv[argn]);
            }
        }
        
        if (patternErrCnt == 0 && optionErrCnt == 0 && fileDirList.size() != 0)
        {
            if (fileDirList.size() == 1 && fileDirList[0] == "-") {
                string filePath;
                while (std::getline(std::cin, filePath)) {
                    std::cerr << "File Matches=" << InspectFiles(filePath) << std::endl;
                }
            } else {
                for (auto const& filePath : fileDirList)
                {
                    std::cerr << "File Matches=" << InspectFiles(filePath) << std::endl;
                }
            }
        }
        
        std::cerr << std::endl;
    }
    
    return 0;
}
