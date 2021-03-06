# **Kickstart**

Small exercises should be as simple in C++ as in Python.

Well that's impossible but this **C++ 17 header library** can help.

There’s a separate “Hello, world!”-document with [detailed installation instructions](documentation/installation/main.md), and there’s also a Work In Progress&trade; [tutorial](documentation/tutorial/main.md).

## ***Example usage:***

~~~cpp
#include <kickstart/all.hpp>
using namespace kickstart::all;

auto main() -> int
{
    const string    name    = input( "Enter your name, please: " );
    const int       age     = to_<int>( input( "What’s your age, in years? " ) );
    
    out << "Greetings, " << age << " years old " << name << "!" << endl;
}
~~~

In [the above code](source/examples/motivational/name-and-age.kickstart.cpp):

* The **`using namespace kickstart::all;`** directive is in practice safe.  
  `using namespace std;` can be a dangerous habit since one risks name collisions. `using namespace kickstart::all` is much more safe. The `kickstart::all` namespace here provides a Python-like UTF-8 input function `input`, a conversion function `to_<int>`, and an UTF-8 stream-like object called **`out`** + an **`endl`**, that correspond roughly to `std::cout` and `std::endl` from the standard library.

* **`input`** inputs a line of text as an UTF-8 encoded string.  
  In particular this means that the user can type in non-English characters such as Norwegian ‘æ’, ‘ø’ and ‘å’.

* **`to_<int>`** converts a string to `int`, or *fails with an exception*.  
  I.e. you never get an arbitrary result on failure, as iostreams give you by default.

Input is expressed as the expression result of a function call `input()` instead of the modification of variables that one gets with iostreams `>>` or `std::getline`. The input-as-expression-result supports use of `const`. And that in turn makes the code shorter and more clear.

Iostreams headers are not included by default because there’s a possible cost of increased size of the executable. Kickstart input and output is instead by default implemented in terms of C `stdin`, `stdout` and `stderr` operations. However, when the input comes directly from a Windows console, or output goes directly to a Windows console, then Kickstart instead uses the UTF-16 based Windows API so as to deal correctly with non-ASCII characters.

## ***Some details:***

* The `to_<int>` function ignores ASCII whitespace on the ends of the string, but other extraneous characters cause failure.

* The `out` stream also supports [ANSI escape sequences](source/examples/console/colored-text.cpp#L9-L13) e.g. for [**colors**](source/library/kickstart/console/ansi_escape_seq.hpp#L102-L149).
 
* The `kickstart::all` namespace also provides [a few select standard library identifiers](source/library/kickstart/core/language/stdlib-includes/basics.hpp#L42-L61). In particular you can use `string`, `array` and `vector` directly instead of writing `std::string`, `std::array` and `std::vector`.

If `to_<int>` ignored all characters after a valid specification then text input `3,14` would erroneously convert to number `3`, and the program could then produce an incorrect but still plausible result. That’s unfortunately the default in the standard library, but Kickstart fixes that for you. If `to_<int>` finds extraneous characters other than leading or trailing whitespace, then it fails with an exception.

The library offers a hierarchy of functionality so that you can choose a suitable level of abstraction and detail for the task at hand. For example, `to_<int>` is just a thin checking wrapper around `to_<double>`, which in turn is effectively an alias for `safe::trimmed_string_to_double`. And for example, `out << v` effectively invokes just `output(str(v))`.

## ***Thanks:***
– Jon Macey [noted](https://www.facebook.com/groups/cppInPracticeQuestions/permalink/3863055490382079/?comment_id=3863694366984858&reply_comment_id=3863759706978324) that clang requires “`= 0`” for the [`bits_per_mantissa_` declaration](source/library/kickstart/core/stdlib-extensions/limits.hpp#L36), plus some more.<br>
– Yahya Mohammed [noted](https://www.facebook.com/groups/cppInPracticeQuestions/permalink/3894807053873589/?comment_id=3896282353726059) a [missing `#include`](source/library/kickstart/system-specific/linux/get_commandline_data.impl.hpp#L35) directive.