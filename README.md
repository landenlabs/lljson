#lljson
OSX / Linux / DOS  Json conversion of arrays to transposed columns in CSV format. 

LLJson parses json files and outputs them as a transposed set of CSV columns. 

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
