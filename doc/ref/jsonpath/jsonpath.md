### jsonpath extension

The jsonpath extension implements [Stefan Goessner's JSONPath](http://goessner.net/articles/JsonPath/).
It provides functions for search and "search and replace" using JSONPath expressions.

This document describes the jsonpath extension since 0.161.0, which is a significant rewrite. Documentation for 
earlier versions may be found [here](https://github.com/danielaparker/jsoncons/blob/v0.160.0/doc/ref/jsonpath/jsonpath.md).
See [JSONPath extension revisited](https://github.com/danielaparker/jsoncons/issues/306) for a review of changes.

### Classes
<table border="0">
  <tr>
    <td><a href="jsonpath_expression.md">jsonpath_expression</a></td>
    <td>Represents the compiled form of a JSONPath string. (since 0.161.0)</td> 
  </tr>
</table>

### Functions

<table border="0">
  <tr>
    <td><a href="make_expression.md">make_expression</a></td>
    <td>Returns a compiled JSONPath expression for later evaluation. (since 0.161.0)</td> 
  </tr>
  <tr>
    <td><a href="json_query.md">json_query</a></td>
    <td>Searches for all values that match a JSONPath expression</td> 
  </tr>
  <tr>
    <td><a href="json_replace.md">json_replace</a></td>
    <td>Search and replace using JSONPath expressions.</td> 
  </tr>
  <tr>
    <td><a href="flatten.md">flatten<br>unflatten</a></td>
    <td>Flattens a json object or array.</td> 
  </tr>
</table>
    
### jsoncons JSONPath

[JSONPath](http://goessner.net/articles/JsonPath/), a loosely standardized syntax for querying JSON, is a creation of Stefan Goessner.
There are many implementations, Christoph Burgmer's [JSONPath comparison](https://cburgmer.github.io/json-path-comparison/)
indicates where they differ and where they agree.

The jsoncons implementation differs from Stefan Goessner's JavaScript implementation in the following respects:

- Stefan Goessner's implemention returns `false` in case of no match, but in a note he suggests an alternative is to return an empty array. 
  The `jsoncons` implementation returns an empty array in case of no match.
- Names in the dot notation may be unquoted (no spaces), single-quoted, or double-quoted.
- Names in the square bracket notation may be single-quoted or double-quoted.
- Wildcards are allowed in the dot notation
- Unions of separate JSONPath expressions are allowed, e.g.

    $..['firstName',@.address.city]

- Options are provided to exclude results corresponding to duplicate paths, and to sort results according to paths.

### Paths

It is a feature of JSONPath that it selects values in the original JSON document, and does not create JSON elements that are not in the original. 
JSONPath uses paths to select values. Paths can use the dot-notation or the bracket-notation.

Select the first (indexed 0) book in [Stefan Goessner's store](https://goessner.net/articles/JsonPath/index.html#e3) using the dot notation:

    $.store.book.0

or

    $.'store'.'book'.0

or

    $."store"."book".0

The leading `$` represents the root JSON value. 
Unquoted names in the dot notation are restricted to digits 0-9, letters A-Z and a-z, 
the underscore character _, and unicode coded characters that are non-ascii. Note 
that names with hyphens must be quoted.

Select the first (indexed 0) book using the bracket-notation: 

    $['store']['book'][0]

or

    $["store"]["book"][0]

Recursively select all book titles under '$.store':

    $.'store'..'title'

Union of a subset of books identified by index:

    $.store[@.book[0],@.book[1],@.book[3]]

Union of the fourth book and all books with price > 10:

    $.store[@.book[3],@.book[?(@.price > 10)]]

JSONPath|       Description
--------|--------------------------------
`$`|    Represents the root JSON value
`@`|    Represents the current node in an expression.
`.` or `[]`|    Child operator
`..`    |Recursive descent. JSONPath borrows this syntax from [E4X](https://en.wikipedia.org/wiki/ECMAScript_for_XML).
`*` |   Wildcard. All objects/elements regardless their names.
`[]`    |Subscript operator. 
`[,]`   |Union.
`[start:end:step]`      |Array slice operator borrowed from ECMASCRIPT 4.
`()`    |Expression.
`?()`   |Filter by expression.

### Duplicates and ordering

Consider the JSON instance 

```json
{
    "books":
    [
        {
            "title" : "A Wild Sheep Chase",
            "author" : "Haruki Murakami"
        },
        {
            "title" : "The Night Watch",
            "author" : "Sergei Lukyanenko"
        },
        {
            "title" : "The Comedians",
            "author" : "Graham Greene"
        },
        {
            "title" : "The Night Watch",
            "author" : "Phillips, David Atlee"
        }
    ]
}
```
with selector
```
$.books[1,1,3].title
```
Note that the second book, _The Night Watch_ by Sergei Lukyanenko, is selected twice.

The majority of JSONPath implementations will produce (with duplicate paths allowed):

Path|Value
-------|------------------
 `$['books'][1]['title']` | "The Night Watch" 
 `$['books'][1]['title']` | "The Night Watch" 
 `$['books'][3]['title']` | "The Night Watch" 

A minority will produce (with duplicate paths excluded):

Path|Value
---------|------------------
`$['books'][1]['title']` | "The Night Watch"
`$['books'][3]['title']` | "The Night Watch"

In 0.161.0, [json_query](json_query.md) defaults to allowing
duplicates, but has an option for no duplicates.
[json_replace](json_replace.md) defaults to no duplicates.

By default, the ordering of results is unspecified, although the user may
expect array ordering at least to be preserved.  In 0.161.0, jsoncons
provides an option for sorting results by paths.

### Slices

jsoncons jsonpath slices have the same semantics as Python slices
(including for negative steps since v0.153.3)

The syntax for a slice is
```
[start:stop:step]
```
Each component is optional.

- If `start` is omitted, it defaults to `0` if `step` is positive,
or the end of the array if `step` is negative.

- If `stop` is omitted, it defaults to the length of the array if `step` 
is positive, or the beginning of the array if `step` is negative.

- If `step` is omitted, it defaults to `1`.

Slice expression|       Description
--------|--------------------------------
`[start:stop]`  | Items `start` through `stop-1`
`[start:]`      | Items `start` to the end of the array
`[:stop]`       | Items from the beginning of the array through `stop-1`
`[:]`           | All items
`[start:stop:step]`|Items `start` up to but not including `stop`, by `step` 

A component `start`, `stop`, or `step` may be a negative number.

Example | Description
--------|------------
$[-1]    | Last item 
$[-2:]   | Last two items
$[:-2]   | All items except the last two
$[::-1]    | All items, reversed
$[1::-1]   | First two items, reversed
$[:-3:-1]  | Last two items, reversed
$[-3::-1]  | All items except the last two, reversed

### Filter predicates

JSONPath uses filter predicates to restrict the set of nodes returned by a path.

[Stefan Goessner's JSONPath](http://goessner.net/articles/JsonPath/) does not provide any specification for the allowable filter expressions, simply stating that expressions can be anything that the underlying script engine can handle. `jsoncons` expressions support the following comparision and arithmetic operators. 

Operator|       Description
--------|--------------------------------
`*`     |Left times right
`/`     |Left divided by right
`+`     |Left plus right
`-`     |Left minus right
`&&`    |Left and right
<code>&#124;&#124;</code>    |Left or right
`==`    |Left is equal to right 
`!=`    |Left is not equal to right
`<`     |Left is less than right
`<=`    |Left is less or equal to right
`>`     |Left is greater than right
`>=`    |Left is greater than or equal to right
'=~'    |Left matches regular expression [?(@.author =~ /Evelyn.*?/)]

Unary operators

Operator|       Description
--------|--------------------------------
`!`     |Not right
`-`     |Negates right

Operator precedence

Precedence|Operator|Associativity
----------|--------|-----------
1 |`!` unary `-`    |Right
2 |`=~`             |Left
3 |`*` `/`          |Left 
4 |`+` `-`          |Left 
5 |`<` `>` `<=` `>=`|Left 
6 |`==` `!=`        |Left 
7 |`&&`             |Left 
8 |<code>&#124;&#124;</code> |Left 

### Unions

In jsoncons, a JSONPath union element can be

- an index or slice expression
- a single quoted name
- a double quoted name
- an expression, e.g. `(@.length-1)`
- a filter
- a wildcard, i.e. `*`
- a path relative to the root of the JSON document (begins with `$`)
- a path relative to the current node (begins with `@`)

To illustrate, the path expression below selects the second, third, and fourth titles from [Stefan Goessner's store](https://goessner.net/articles/JsonPath/index.html#e3):

```
"$.store.book[:-2:1,(@.length-2),?(@.author=='J. R. R. Tolkien')].title"
```

### Function expressions

Support for function expressions is a jsoncons extension.

Functions can be passed JSONPath paths, single quoted strings and literal JSON values
such as `1.5`, `true`, or `{"foo" : "bar"}`. 
Outside a filter predicate, functions can be passed paths that select from
the root JSON value `$`. Within a filter predicate, functions can be passed either a 
path that selects from the root JSON value `$`, or a path that selects from the current node `@`.

Function|Description
----------|--------
[abs](functions/abs.md)|Returns the absolute value of a number.
[avg](functions/avg.md)|Returns the average of the items in an array of numbers.
[ceil](functions/ceil.md)|Returns the smallest integer value not less than a given number.
[contains](functions/contains.md)|Returns true if a source array contains a search value, or a source string contains a search string.
count (until 0.161.0)|Same as [length](functions/length.md)
[ends_with](functions/ends_with.md)|Returns true if the source string ends with the suffix string, otherwise false.
[floor](functions/floor.md)|Returns the largest integer value not greater than a given number.
[keys](functions/keys.md)|Returns an array of keys in an object.
[length](functions/length.md)|Returns the length of an array, object or string.
[max](functions/max.md)|Returns the highest number found in an array of numbers,or the highest string in an array of strings.
[min](functions/min.md)|Returns the lowest number found in an array of numbers, or the lowest string in an array of strings.
[prod](functions/prod.md)|Returns the product of the items in an array of numbers.
[starts_with](functions/starts_with.md)|Returns true if the source string starts with the prefix string, otherwise false.
[sum](functions/sum.md)|Returns the sum of the items in an array of numbers.
[to_number](functions/to_number.md)|If string, returns the parsed number. If number, returns the passed in value.
[tokenize](functions/tokenize.md)|Returns an array of strings formed by splitting the source string into an array of strings, separated by substrings that match a given regular expression pattern.

### Examples

The examples use the sample data file `books.json`, 

```json
{
    "books":
    [
        {
            "category": "fiction",
            "title" : "A Wild Sheep Chase",
            "author" : "Haruki Murakami",
            "price" : 22.72
        },
        {
            "category": "fiction",
            "title" : "The Night Watch",
            "author" : "Sergei Lukyanenko",
            "price" : 23.58
        },
        {
            "category": "fiction",
            "title" : "The Comedians",
            "author" : "Graham Greene",
            "price" : 21.99
        },
        {
            "category": "memoir",
            "title" : "The Night Watch",
            "author" : "Phillips, David Atlee"
        }
    ]
}
```

#### json_query function

```
#include <fstream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpath/jsonpath.hpp>

using json = jsoncons::json;
namespace jsonpath = jsoncons::jsonpath;

int main()
{
    std::ifstream is("./input/books.json");
    json data = json::parse(is);

    auto result1 = jsonpath::json_query(data, "$.books[1,1,3].title");
    std::cout << "(1)\n" << pretty_print(result1) << "\n\n";

    auto result2 = jsonpath::json_query(data, "$.books[1,1,3].title",
                                        jsonpath::result_options::path);
    std::cout << "(2)\n" << pretty_print(result2) << "\n\n";

    auto result3 = jsonpath::json_query(data, "$.books[1,1,3].title",
                                        jsonpath::result_options::value | jsonpath::result_options::nodups);
    std::cout << "(3)\n" << pretty_print(result3) << "\n\n";

    auto result4 = jsonpath::json_query(data, "$.books[1,1,3].title",
                                        jsonpath::result_options::nodups);
    std::cout << "(4)\n" << pretty_print(result4) << "\n\n";
}
```
Output:
```
(1)
[
    "The Night Watch",
    "The Night Watch",
    "The Night Watch"
]

(2)
[
    "$['books'][1]['title']",
    "$['books'][1]['title']",
    "$['books'][3]['title']"
]

(3)
[
    "The Night Watch",
    "The Night Watch"
]

(4)
[
    "$['books'][1]['title']",
    "$['books'][3]['title']"
]
```

#### json_replace function

```
#include <fstream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpath/jsonpath.hpp>

using json = jsoncons::json;
namespace jsonpath = jsoncons::jsonpath;

int main()
{
    std::ifstream is("./input/books.json");
    json data = json::parse(is);

    auto f = [](const std::string& /*path*/, json& price) 
    {
        price = std::round(price.as<double>() - 1.0);
    };
    jsonpath::json_replace(data, "$.books[*].price", f);

    std::cout << pretty_print(data) << "\n";
}
```
Output:
```
{
    "books": [
        {
            "author": "Haruki Murakami",
            "category": "fiction",
            "price": 22.0,
            "title": "A Wild Sheep Chase"
        },
        {
            "author": "Sergei Lukyanenko",
            "category": "fiction",
            "price": 23.0,
            "title": "The Night Watch"
        },
        {
            "author": "Graham Greene",
            "category": "fiction",
            "price": 21.0,
            "title": "The Comedians"
        },
        {
            "author": "Phillips, David Atlee",
            "category": "memoir",
            "title": "The Night Watch"
        }
    ]
}
```

#### jsonpath_expression

A [jsoncons::jsonpath::jsonpath_expression](jsonpath_expression.md) 
represents the compiled form of a JSONPath expression. It allows you to 
evaluate a single compiled expression on multiple JSON documents.
A `jsonpath_expression` is immutable and thread-safe. 

```
#include <fstream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpath/jsonpath.hpp>

using json = jsoncons::json;
namespace jsonpath = jsoncons::jsonpath;

int main()
{
    auto expr = jsonpath::make_expression<json>("$.books[1,1,3].title");

    std::ifstream is("./input/books.json");
    json data = json::parse(is);

    json result1 = expr.evaluate(data);
    std::cout << "(1)\n" << pretty_print(result1) << "\n\n";

    json result2 = expr.evaluate(data, jsonpath::result_options::path);
    std::cout << "(2)\n" << pretty_print(result2) << "\n\n";

    json result3 = expr.evaluate(data, jsonpath::result_options::value | jsonpath::result_options::nodups);
    std::cout << "(3)\n" << pretty_print(result3) << "\n\n";

    json result4 = expr.evaluate(data, jsonpath::result_options::nodups);
    std::cout << "(4)\n" << pretty_print(result4) << "\n\n";
}
```
Output:
```
(1) 
[
    "The Night Watch",
    "The Night Watch",
    "The Night Watch"
]

(2) 
[
    "$['books'][1]['title']",
    "$['books'][1]['title']",
    "$['books'][3]['title']"
]

(3) 
[
    "The Night Watch",
    "The Night Watch"
]

(4) 
[
    "$['books'][1]['title']",
    "$['books'][3]['title']"
]
```

