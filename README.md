<table border="0">
  <tr>
    <td>
      <!-- VERSION -->v6.07.10<br>
      <!-- DATE -->12-Jul-2026<br>
      macOS<br>
      <a href="https://landenlabs.com">Home</a>
    </td>
    <td>
      <a href="https://landenlabs.com">
        <img src="screens/landen_labs_300.webp" width="300" alt="LanDen Labs">
      </a>
    </td>
  </tr>
</table>

# lljson
OSX / Linux / DOS  Json conversion of arrays to transposed columns in CSV format. 

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE.txt)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows-lightgrey.svg)

LLJson parses json files and outputs them as a transposed set of CSV columns. 

### Dependencies
* [llcommon](https://github.com/landenlabs/llcommon) - shared LanDen Labs utility library (git submodule)

This input json file:

<pre>
{
  "quiz": {
    "sport": {
      "q1": {
        "question": "Which one is correct team name in NBA?",
        "options": [
          "New York Bulls",
          "Los Angeles Kings",
          "Golden State Warriros",
          "Huston Rocket"
        ],
        "answer": "Huston Rocket"
      }
    },
    "maths": {
      "q1": {
        "question": "5 + 7 = ?",
        "options": [
          "10",
          "11",
          "12",
          "13"
        ],
        "answer": "12"
      },
      "q2": {
        "question": "12 - 8 = ?",
        "options": [
          "1",
          "2",
          "3",
          "4"
        ],
        "answer": "4"
      }
    }
  }
}
</pre>

Is converted to CSV columns as:
<pre>
quiz.maths.q1.options, quiz.maths.q2.options, quiz.sport.q1.options
"10", "1", "New York Bulls"
"11", "2", "Los Angeles Kings"
"12", "3", "Golden State Warriros"
</pre>

Visit home website

[https://landenlabs.com](https://landenlabs.com)


Help Banner:
<pre>
lljson  Dennis Lang v1.1 (landenlabs.com) Dec 20 2019

Des: Json parse and output as transposed CSV
Use: lljson [options] directories...   or  files

 Options (only first unique characters required, options can be repeated):
   -includefile=&lt;filePattern>
   -excludefile=&lt;filePattern>
   -verbose

 Example:
   lljson -inc=*.json -ex=foo.json -ex=bar.json dir1/subdir dir2 file1.json file2.json
 Example input json:
   {
      "cloudCover": [
        10,
        30,
        49
      ],
        "dayOfWeek": [
        "Monday",
        "Tuesday",
        "Wednesday"
      ]
   }

   Output transposed CSV
    cloudCover,  dayOfWeek
     10, Monday
     30, Tuesday
     49, WednesDay

</pre>

### License

```
Copyright 2026 Dennis Lang

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
See [LICENSE.txt](LICENSE.txt) for the full license text.
