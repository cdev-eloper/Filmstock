# Film Stock Language Documentation

<p align="center">
  <img src="bonus/logo.png" width="500">
</p>

Film Stock is the new up and coming, blazingly-fast™🚀🚀🚀🔥🔥🔥 language with ["""C-like"""](#bench) performance.

On a serious note, Film Stock is a very simple esolang that revolves around not having a stack at all and being untyped.

## Table of contents

- [Project Structure](#struct)
- [Usage and Installation](#usage)
- [Examples](#ex)
- [The language](#about)
- [Built-in Functions](#funcs)
- [VM and data formats](#vm)
- [Tips and Tricks](#tips)
- [Few words on the Compiler's madness](#comp)
- [Highlighting tools](#tools)
- [Benchmarks](#bench)

## Project Structure
<a id="struct"></a>

- **`bin/`**: Pre-compiled binaries. Includes `Filmstock.jar` (Compiler / Assembler) and `runner`/`runner.exe` (VM).
- **`bonus/`**: Contains Highlighting tools, source code, the logo and a key look up html.
- **`examples/`**: Sample programs including my 3D renderer and rb sim.
- **`src/`**: Default directory for source code. Edit `main.fss` here to use the quick-run scripts.
- **`run` / `debug`**: Quick-start scripts that compile `src/main.fss` and execute it immediately. Use .sh for Linux and .bat for Windows.

## Usage / Installation
<a id="usage"></a>

### Dependencies

Linux:
- XWayland or X11
- GLIBC >= 2.29
- JRE >= 17 (Compiler Only)

Windows:
- JRE >= 17 (Compiler Only)

For compiling the VM, you need a C++ 17 (or higher) compiler.

### Installation

Everything works out of the box, simply download [a release](https://github.com/cdev-evloper/Filmstock/releases) and make sure that you have Java JRE 17+ installed.

The language consists of two parts, the compiler and the VM.

### Compiler

The compiler is written in Java and requires at least JDK-17.
It can assemble Filmstock assembly and compile Filmstock Scripts and is located in `bin/Filmstock.jar`.
Use `java -jar Filmstock.jar -help` for more details.

### VM / Runner

The C++ binary was compiled for x64 Windows and Linux.
It can run Filmstock bytecode and is located in `bin/runner` or `bin\runner.exe`.
Use `./runner PATH_TO_BYTECODE` or `runner.exe PATH_TO_BYTECODE` to execute bytecode.
On Linux, do not forget to allow the VM to be executed as a program. This can be achieved with `chmod +x runner`.

### Built-in Extras

There is a building script located in the root called `run.bat` or `run.sh` depending on your operating system.
It compiles the source code from `src/main.fss` to `out/build.filmstock` and executes it directly.

There is a second script called `debug`, which does the same as the `run` script but with debugging and no optimizations.
As such, you must press Enter between the compilation and execution. This allows you to see the ASTs and debug your code. The debug script also generates a .fsdbi file.

For comfort I also highly recommend the [Highlighting tools](#tools).

### Compiler CLI overview

```
You can use multiple commands after one another to queue tasks.

-help: Prints this.
-build    <inpath> <outpath> : Compiles and assembles a program.
-compile  <inpath> <outpath> : Compiles Filmstock into Filmstock Assembly with a debug info file.
-assemble <inpath> <outpath> : Assembles Filmstock Assembly into bytecode.
-O         <0-N / help>      : Sets the optimization level or prints information about optimizations.
-debug                       : Verbose: When compiling, you can use this
                               flag to debug a lot of info about the next Compilation / Assembly.
                                -> Used for quick debugging.
-no-info                     : Disables building the debug info (.fsdbi) file.
```

### Examples
<a id="ex"></a>

To test the examples, simply copy the source code from an example directory into the src directory.
Finally run the "run" script located in the root directory.

There are also prebuilt filmstock binaries for each example in the directories labled `build.filmstock`.

The _basic directory contains all the examples which showcase the language.
Everything interesting is in the root example folder.

```
Cool examples:
- balls: 2d Circle Rigidbody Sim with some fancy graphics
-> Controls: WASD, scrolling

- brainfucker: Brainfuck transpiler in the macro system

- 3d_renderer: 3D Renderer of a model; Simple shading
-> Controls: Mouse dragging, scrolling

- icosphere: 3D Renderer / Light engine / Icosphere generation
-> Real time shading demo of my not very optimized engine
  -> The white cube is the source of light
-> An icosphere bounces around with some squash and stretch
-> Controls: Mouse dragging, scrolling. S for subdividing, R for reverting subdivision.
```

## The language
<a id="about"></a>

## Limitations

Due to developer ~~laziness and unintelligence~~ creativity, I added a few limitations to the language to keep things "interesting":

- No Stack: All temporaries are statically allocated.
- No Functions: Personally, I do not use any functions at all but you can use a heap list as a stack for recursion, have fun :)

Because the compiler is "a work of art", the error messages can be cryptic. In these cases, post an Issue and I will fix it. Please build with "-debug" so I can see the stack trace.

## Ways to Operate with Data

- Values `0-255` are interpreted as UTF-8 characters in `print()` (overflowed values use modulo 256).
- "Booleans": `0` is false; `1` or any `non-zero` value is true.
- List pointers are used to reference lists in memory (yes, the doubles are truncated for internal operations).

## Syntax

- Statements are separated by new lines. You may leave as many new lines as you like. This is functionally equivalent to Java's `;`.
- Variable names cannot include any of the following characters: `$ # @ = & | ? > < ^ * / + \ - % ( ) { } ; , [ ] .` or a **new line**.
- You may use any amount of new lines when using `{` and `}`.

## Data formats

- Film Stock Script files are written in `.fss` Files.
- Human Readable IR files are written in `.aroll` Files.
- Film Stock Executable / Bytecode files are written in `.filmstock` Files.
- Film Stock Debug Information is stored in `.fsdbi` Files. They simply include the lines and ids of lines corresponding to each operation, separated with new lines.

### Statements

```
#include PATH                   - Add all lines at position from PATH.

var VARIABLE_NAME = EXPRESSION - Initialize a variable with the value of EXPRESSION.

var VARIABLE_NAME              - Initialize a variable with 0.

VARIABLE_NAME = EXPRESSION     - Set an existing variable to the value of EXPRESSION.

//COMMENT                      - Single-line comments are written with '//'.

var a \                        - "\" can be used to split a long line of code into multiple.
= 10

'a'                            - Sugar: character literal. Evaluates to the ASCII code of 'a'
                                 Use \' to escape a single-quote inside

"Fake String"                  - Sugar: string literal. Interpreted as the ASCII code of each character
                                 (implementation detail: string-literals are expanded into a sequence of ascii values).
                                 Use \" to escape a double-quote inside a string.

\n, \\, \=, \.                 - Escape sequences: in order: newline symbol, backslash symbol,
                                 equals symbol, dot symbol.

if CONDITION {                 - If statement (body required inside { }). Executed when CONDITION is non-zero.
    // Executed if condition is non-zero
} else {
    // Executed if condition is zero
}

for INIT; COND; UPDATE {       - Classic for loop. INIT executes once before loop (can declare variables).
    // Body executes while COND is non-zero. UPDATE runs after each iteration.
}                              - This creates a new scope for the for and for the body.

break                          - Immediately exits the nearest enclosing loop.
continue                       - Skips the rest of current loop iteration.

{
    // New scope               - Creates a new scope. You can shadow variable names here.
}
```

### Arithmetic Expressions

```
FUNCTION_NAME(ARGUMENTS)       - Call a built in function.

(EXPRESSION)                   - Parentheses for grouping

V1^V2                          - Exponentiation.
                                -> For negative bases and non-integer exponents: Only works on bases, which are 1 / b.
                                  -> Messes up the sign of a / b exponents. Also results are "debatable" when there
                                     is no real solution.
                                  -> This is because approximating a ratio is very expensive
                                     for a general purpose function.

V1*V2                          - Multiplication

V1/V2                          - Division
                                  - Division by 0 produces NaNs.

V1%V2                          - Negative safe modulo
                                  - The result always has the same sign as base.
                                  - Modulo division by 0 produces NaNs.

V1+V2                          - Sum

V1-V2                          - Difference

V1&V2                          - Logical AND: returns 1 if both V1 and V2 are non-zero, else 0. No short circuits.

V1|V2                          - Logical OR: returns 1 if V1 or V2 is non-zero, else 0. No short circuits.

V1?V2 or V1==V2                - Equality: returns 1 if V1 equals V2, else 0.

V1>V2                          - Greater than: returns 1 if V1 > V2, else 0.

V1<V2                          - Less than: returns 1 if V1 < V2, else 0.

V1>=V2                         - Greater than or equal: returns 1 if V1 >= V2, else 0.

V1<=V2                         - Less than or equal: returns 1 if V1 <= V2, else 0.

V1!=V2                         - Nonequality: returns 0 if V1 equals V2, else 1.
```

## Built-in Functions
<a id="funcs"></a>

```
pi()                                   - Returns the mathematical constant Pi.

sin, cos, tan (radian)                 - Standard trigonometric functions (arguments in radians, or)

asin, acos (ratio)                     - Standard trigonometric arcsine and arccosine (input between -1 and 1)

atan2(y, x)                            - Two-argument arctangent: atan2(y, x)

root(a, n)                             - Returns the nth root of a

sqrt(a)                                - Returns the square root of a.

not(a)                                 - Returns 0 if a is non-zero, else 1

round(a)                               - Returns rounded value

ceil(a)                                - Returns ceiled value

floor(a)                               - Returns floored value

time()                                 - Returns system time in milliseconds since epoch

print(a, b, c, ...)                    - Interpret each numeric argument as an ASCII index
                                         and print/emit characters:
                                         -> for a, it resolves the character with index floor(a % 256)
                                         -> Implementation limitation: due to the VM API this
                                            function returns a copy of the first numeric argument
                                            rather than a separate string object.
                                         -> Does not end with a new line!

printNum(a, b, c, ...)                 - Prints numeric arguments in decimal notation with
                                         2 decimal precision, separated by ", ".
                                         -> Implementation limitation: returns a copy of the first argument.
                                         -> Does not end with a new line!

nl()                                   - Prints a new line.


sleep(a)                               - Sleeps for a milliseconds

random()                               - Returns a non-deterministic random floating value in [0, 1)


THE ONLY FUNCTIONS WHICH CHANGE THE VARIABLE VALUES INSIDE THEM:

it(a)                                  - "Iterate": increase value of a by 1 and return a copy of the new value

ia(a)                                  - "Iterate After": return a copy of a, then increase a by 1
```

## Macros and Defines

```
#define token VALUE                    - Replaces the token everywhere in the code after the define.
                                         Use \n for new lines in the define.

#macro NAME(ARG1, ARG2, ..., ARGN) {   - Defines a macro with arguments
    // Macro body                       -> All $[ARGUMENT NAME] will be replaced with the input
    // Use $ARG1 to reference ARG       -> The replacements are executed left to right
}

#NAME(VALUE1, VALUE2, ..., VALUEN)     - Expands the macro at this position.
                                         Arguments are substituted textually
                                         (pure textual substitution of $ARGi with VALUEi).
                                         This creates a new context for calling a macro.

##NAME(VALUE1, VALUE2, ..., VALUEN)    - Same as #macro calling but does not create a new context.

#macro testMacro(...) {                - Place __ in a macro to replace it with all capital macro name + "MACRO"
    var __name = 0                     - This can be used when you want to avoid variable shadowing.
    print("__")

    print("To print two _ in a row, use: _", '_')
}

```

## List Functions

### Filmstock's List System: THE MATRIX

- The matrix is a list of lists.
- When you allocate a list with lnew(), it gets added at the end of the list of lists.
- You access the individual indexes with the "pointers" (indexes in THE MATRIX).
- You cannot free lists. You can nuke all lists with lreset(), which resets everything.

```
lamount()                      - Returns the amount of lists total

lreset()                       - "Resets" the list system. Removes all lists and frees pointers

lnew()                         - Creates a new List and returns the pointer

lget(pointer, id)              - Returns value from a list at position id (0-based indexing)
                                 -> Use listName[id] for easier access

lset(pointer, id, val)         - Sets the value at index to value
                                 -> Use listName[id] = for easier setting

lsetTo(pointer, values...)     - Sets the whole list to the values listed,
                                 like lsetTo(pointer, "Hello World!\n")
                                 -> Use listName.setTo for easier setting
                                 -> If a line with lsetTo is longer than 1000 characters,
                                    it gets compiled as a fast lsetTo statement.
                                 --> In fast lsetTo statements you
                                     may only use constant numeric values.

llen(pointer)                   - Returns the length of the list referenced by pointer
                                 -> Use listName.len()

ladd(pointer, value)           - Adds value to list
                                 -> Use listName.add(value)

lremove(pointer, index)        - Removes value at index from list
                                 -> Use listName.remove(index)

lempty(pointer)                - Empties the list
                                 -> Use listName.empty()

linsert(pointer, value, index) - Inserts value at index
                                 -> Use listName.insert(value, index)

lreverse(pointer)              - Reverses list
                                 -> Use listName.reverse()

lshuffle(pointer)              - Shuffles list
                                 -> Use listName.shuffle()

lsort(pointer, type)           - Sorts list. Type 0 for ascending, else descending
                                 -> Use listName.sort(type)

lprint(pointer)                - Equivalent to calling print for each element of the list
                                 -> Use listName.print()
                                 -> Does not end with a new line!

lprintNum(pointer)             - Prints the numbers in a list separated by
                                 , and surrounded by square brackets.
                                 -> Use listName.printNum()
                                 -> Does not end with a new line!

lprintSep(pointer, sep, f)     - Same as print but prints a sep character each f
                                 -> Use listName.printSep(sep, f)
                                 -> Does not end with a new line!

linput(pointer)                - Halts code, gets one line of terminal input and appends it
                                 to a list the input as well as a newline character.

lload(data, path)              - Loads the contents as "chars" from the path stored in a string.
                                 -> Use listName.load(path)

lstore(data, path)             - Stores the contents as "chars" to the path stored in a string.
                                 -> Use listName.store(path)
```

## Raylib Functions

```
dStart()                       - Start sending draw instructions

dEnd()                         - End sending draw instructions -> draw on screen

setColor(r, g, b, a)           - Set the color that is used to draw (0-255)

setPosition(x, y)              - Set drawing offset (0, 0 is top left) in px

setTextSize(size)              - Set text size in px

ldrawTriangles(pointer)        - Draw a list as triangles
                                 -> Use listName.drawTriangles()
                                 -> Every 6 values are interpreted as:
                                 -> x1, y1, x2, y2, x3, y3
                                 -> Winding does not matter.

ldrawText(pointer)             - Draw a list as text
                                 -> Use listName.drawText()
                                 -> Text is interpreted just like in print()

isPressed(id)                  - Returns 1 if key is pressed, else 0
                                 -> Text is interpreted just like in print()
                                 -> Use /bonus/key_lookup.html as an id lookup.

isMousePressed(button)         - Returns 1 if the given mouse is pressed, else 0
                                 -> button = 0: Left mouse button
                                 -> button = 1: Right mouse button
                                 -> button = 2: Middle mouse button

mouseX() / mouseY()            - Get Window coordinate of the mouse

getScroll()                    - Get Mouse wheel information (frame dependent)
                                 -> Up: positive, Down: Negative

width()                        - Returns window width in px

height()                       - Returns window height in px
```

## VM
<a id="vm"></a>

### Memory

| Name | Type | Description |
|---|---|---|
| `film` | `double[]` | Memory for all temporaries. All variables live here, addressed by integer index and statically allocated. |
| `matrix` | `vector<vector<double>>` | Lists are created at runtime and referenced by integer ID stored in film. Here bound checks are performed. More in the list section. |

### Bytecode

In memory operations are stored as:

- opcode (4 Bytes)
- arg1 (4 Bytes)
- arg2 (4 Bytes)
- arg3 (4 Bytes)

Here are short descriptions of what each bytecode operations do, generated by an LLM because I changed this table at least a half dozen times and it is a horror to rewrite:

```
COPY                    arg2 = arg1
ADD                     arg3 = arg1 + arg2
SUBTRACT                arg3 = arg1 - arg2
MULTIPLY                arg3 = arg1 * arg2
DIVIDE                  arg3 = arg1 / arg2
SQUARE_ROOT             arg2 = √arg1
SIN                     arg2 = sin(arg1)
COS                     arg2 = cos(arg1)
TAN                     arg2 = tan(arg1)
ASIN                    arg2 = asin(arg1)
ACOS                    arg2 = acos(arg1)
ATAN2                   arg3 = atan2(arg1, arg2)
POWER                   arg3 = arg1 ^ arg2
MOD                     arg3 = arg1 % arg2
IS_EQUAL                arg3 = arg1 == arg2
IS_GREATER              arg3 = arg1 > arg2
NOT                     arg2 = arg1 == 0 ? 1 : 0
AND                     arg3 = arg1 != 0 && arg2 != 0
OR                      arg3 = arg1 != 0 || arg2 != 0
JUMP_IF                 if arg1 != 0: jump to instruction arg2
PRINT                   print chars from film[arg1..arg1+arg2]
PRINT_NUMBERS           print film[arg1..arg1+arg2] as numbers
ITERATE                 arg1 += 1
COPY_FROM               arg2 = film[arg1]  (indirect/pointer read)
POINTER                 arg2 = arg1  (stores instruction index as value)
POSITION                arg1 = currentInstruction
JUMP                    jump to instruction arg1
END                     exit with code arg1
FLOOR                   arg2 = floor(arg1)
ROUND                   arg2 = round(arg1)
CEIL                    arg2 = ceil(arg1)
TIME                    arg1 = current time in ms
NEW_LIST                create new list; arg1 = its index
COPY_FROM_LIST          arg3 = matrix[arg1][arg2]
LENGTH_OF_LIST          arg2 = len(matrix[arg1])
LIST_AMOUNT             arg1 = number of lists
ADD_LIST                matrix[arg1].push(arg2)
REMOVE_AT_LIST          matrix[arg1].remove(arg2)
EMPTY_LIST              matrix[arg1].clear()
REMOVE_ALL_LISTS        matrix.clear()
ADD_AT_LIST             matrix[arg1].insert(arg3, at: arg2)
REVERSE_LIST            matrix[arg1].reverse()
SHUFFLE_LIST            matrix[arg1].shuffle()
SORT_LIST               matrix[arg1].sort(); arg2=1 → descending
SLEEP                   sleep arg1 ms
RANDOM                  arg1 = random float [0, 1)
INPUT                   read line into matrix[arg1] as char codes
LIST_SET                matrix[arg1][arg2] = arg3
UPDATE_CONSOLE          flush print buffer to stdout
PRINT_VECTOR            print matrix[arg1] as chars
PRINT_VECTOR_NUMBERS    print matrix[arg1] as [n, n, ...]
PRINT_VECTOR_SEPARATED  print matrix[arg1] as chars, separator arg2 every arg3 elements
SET_COLOR               set draw color to arg1
SET_POSITION            set cursor/draw position to (arg1, arg2)
SET_TEXT_SIZE           set text size to arg1
DRAW_TRIANGLES          draw triangles from vertex data in matrix[arg1]
DRAW_TEXT               draw string from matrix[arg1] at current position
BEGIN_DRAWING           begin frame; exit if window closed
END_DRAWING             end frame / present
IS_PRESSED              arg2 = keyboard key arg1 is held
MOUSE_PRESSED           arg2 = mouse button arg1 is held (0=L, 1=R, 2=M)
GET_MOUSE_X             arg1 = mouse X
GET_MOUSE_Y             arg1 = mouse Y
GET_SCROLL              arg1 = mouse wheel delta
GET_WIDTH               arg1 = window width
GET_HEIGHT              arg1 = window height
LIST_LOAD               read file named by matrix[arg2] into matrix[arg1]
LIST_STORE              write matrix[arg1] as chars to file named by matrix[arg2]
```

### File formats

#### .filmstock / Executable

| Field | Size | Description |
|---|---|---|
| `film_length` | 4 bytes (int) | Total number of `film` slots to allocate. |
| Initial values | 12 bytes × amount of initial values | Sparse non-zero `film` entries. Each entry: 4-byte int index + 8-byte float value. |
| Margin | 64 × `0xFF` | Separator between data and code segments. |
| Instructions | 16 bytes × Amount instructions | Encoded instructions. See instruction format. |

#### .fsdbi / Debug Information

This is a very simple text data format. When a foreseeable error (not something like a segfault because of driver failures) happens, it prints the debug information. For each bytecode instruction in an executable file it stores:

- Text: The "Stacktrace" of macros / files that resulted in producing this line of code
- Newline
- Text: The code of the line
- Newline

#### .aroll / Human Readable IR

These files are 1:1 translated to Filmstock executable files. You can edit these files per hand. In the beginning you specify the size of the film and use "set INDEX VALUE" statements to set the starting values of some variables. Here is the IR format for a the `_basic/list_basics/` example. Use bytecode names and specification above.

```
length = 24
set 13 10.0
set 14 72.0
set 15 101.0
set 16 108.0
set 17 111.0
set 18 32.0
set 19 87.0
set 20 114.0
set 21 100.0
set 22 0.0
NEW_LIST 23
PRINT_NUMBERS 23 1
UPDATE_CONSOLE
PRINT 13 1
UPDATE_CONSOLE
COPY 23 0
COPY 14 1
COPY 15 2
COPY 16 3
...
```

## Tips and tricks
<a id="tips"></a>

General:

- Use the debug script or build with '.fsdbi' generation enabled when stuck.
- Use as many macros as possible; Use macros as you would use functions.
- Write reusable code, more abstraction through more macro parameters. Future you will be thankful.
- Separate code into different parts, like an assembler dev: Each part should have a clear job.

Lists:

- Have dedicated temp lists.
- Document your data structures, future you will be VERY thankful.
- Use lists as "lists of structs". So for points you'd have a [X, Y, Z, X, Y, Z, ...].

Graphics:

- Create a helper that can create the shape you want from triangles, generalize it somewhat.
- Use setPosition for simple camera offsets / text rendering.
- If your graphics code is repetitive, you're not using enough macros.

## About graphics

As you might've noticed, the graphics API is severely limited. This is by design. Writing graphics
only using triangles and will feel rewarding once you get the hang of it. Additionally you will understand more of how graphics
actually work.

How I recommend thinking about it is laying out what you want to draw, then splitting the whole
into more primitive shapes and finally implementing these primitive shapes with triangles.

For example for my 'balls' program, I decided that I want 2D balls stretching and squashing in a box.
As such it becomes apparent that I need these two things:

- Creating circles and then stretching them.
- Drawing rectangles.

The rectangle part is not so bad, we simply must render two triangles using the rect's corners.

```js
var clearScreen = lnew()

#macro fill(x, y, w, h, r, g, b, a) {
    clearScreen.setTo($x, $y, $x, $y+$h, $w+$x, $y, $w+$x, $h+$y, $w+$x, $y, $x, $h+$y)
    setColor($r, $g, $b, $a)
    clearScreen.drawTriangles()
}
```

The circles themselves are not that simple but still simple.

I decided to approximate the circle simply using a regular polygon. As such
we walk around the perimeter in set angle steps and place points. We connect neighboring points to each other and to the center.

```js
#macro setToRegularPolygon(list, index, vertices, size, middleX, middleY) {
    // Step per iteration
    var __step = pi() * 2 / $vertices
    // Starting angle
    var __angle = 0

    for var __s = 0; __s < $vertices ; it(__s) {
        // triangulation of the regularPolygon
        $list[__s*6+0+$index] = $middleX
        $list[__s*6+1+$index] = $middleY
        $list[__s*6+2+$index] = $middleX + cos(__angle) * $size
        $list[__s*6+3+$index] = $middleY + sin(__angle) * $size
        $list[__s*6+4+$index] = $middleX + cos(__angle-__step) * $size
        $list[__s*6+5+$index] = $middleY + sin(__angle-__step) * $size

        __angle = __angle - __step
    }
}
```

I did end up implementing more transformations and etc. but these two building blocks allowed me to build the whole program.

## Few words on the Compiler's madness
<a id="comp"></a>

The compiler is a work of art. Without doubt, it is one of the most "interesting" pieces of code conceived.

This compiler was first written when I did not know anything about "state machines" or "languages". Everything here was made from scratch and every idea is a product of my thinking.
There is no parsing or lexing and everything is done with string operations.

As such the pipeline for codegen is as follows:

- Reading the main source file.
- Inlining all included files recursively.
- Replace the "tokens", which are overriden by #define.
- Check if lines end in ";" and throw an error. Handle assignments without a "= value". Replace "==", "!=", etc. with unique symbol combinations.
- Replace ">=", etc. with "not(... < ...)".
- Put all braces in their own lines.
- Put fors in their own contexts.
- Parse Macros.
- Inline Macros.
- Replace compound assigments with their counterparts.
- Replace "list[index] = ..." with "lset(list, index, ...)".
- Replace "list[index]" with "lget(list, index)".
- Replace "list.func()" with "lfunc(list, ...)".

Now we simply create an AST of each line, handle 'for', 'if', 'continue' and 'break' with special cases and compile like any sane compiler.

## Highlighting Tools
<a id="tools"></a>

## VS Code

Find your extensions folder:
- Windows: `%USERPROFILE%\.vscode\extensions`
- Linux: `~/.vscode/extensions`

Copy the entire `Filmstock` folder from `/bonus/highlighting/` to the extensions folder.
Restart VS code or Ctrl+Shift+P.

## IntelliJ-based IDEs

In your IDE go to `Settings/Preferences → Editor → TextMate Bundles`.
Click +, browse to and select the `Filmstock.tmbundle` folder from `/bonus/highlighting/`.
Apply.

## Kate

#### Windows

- Copy the XML from this `bonus/highlighting/filmstock.xml` into `%USERPROFILE%\AppData\Local\org.kde.syntax-highlighting\syntax`.
- If the path does not exist, you must create the missing folders.

#### Linux

- Copy the XML from this `kate/filmstock.xml` into the fitting folder:
  - Local user (tested on Arch): `$HOME/.local/share/org.kde.syntax-highlighting/syntax/`
  - All users: `/usr/share/org.kde.syntax-highlighting/syntax/`
  - Flatpak: `$HOME/.var/app/flatpak-package-name/data/org.kde.syntax-highlighting/syntax/`
  - Snap: `$HOME/snap/snap-package-name/current/.local/share/org.kde.syntax-highlighting/syntax/`
- If the path does not exist, you must create the missing folders.

---

## Benchmarks
<a id="bench"></a>

Here is a simple benchmark of some popular languages on my PC to get a taste of Filmstock's performance.
Here we simply add the first billion numbers. I used doubles in C and Java to get a better comparison.
I wanted to compare how fast the "interpreters" of each language are (C's interpreter is my CPU), not how good the vectorization or parallelization of each language is or
who has the most specialized bytecode.
As such I set the number as volatile in C and used a manual loop in Python and tested Java with and without JIT.

### Java

```java
public class AddNumbers {
    public static void main(String[] args) {
        long startTime = System.currentTimeMillis();

        double total = 0.0;
        int i = 0;
        while (i < 1000000000) {
            total = total + i;
            i = i + 1;
        }

        long endTime = System.currentTimeMillis();
        long elapsed = endTime - startTime;

        System.out.println("Total sum: " + total);
        System.out.println("Time taken: " + elapsed + " ms");
    }
}
```

```
java AddNumbers
Total sum: 4.99999999067109E17
Time taken: 777 ms

java -Xint AddNumbers
Total sum: 4.99999999067109E17
Time taken: 13874 ms
```

### C

```c
#include <stdio.h>
#include <sys/time.h>

int main() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    volatile double total = 0.0;
    int i = 0;
    while (i < 1000000000) {
        total = total + i;
        i = i + 1;
    }

    gettimeofday(&end, NULL);

    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 +
                      (end.tv_usec - start.tv_usec) / 1000;

    printf("Total sum: %.0f\n", total);
    printf("Time taken: %ld ms\n", elapsed_ms);

    return 0;
}
```

```
gcc -O3 -o add_numbers main.c
./add_numbers
Total sum: 499999999067108992
Time taken: 2590 ms
```

### Python

```python
import time

start_time = time.time()

total = 0
i = 0
while i < 1000000000:
    total = total + i
    i = i + 1

end_time = time.time()

elapsed_ms = (end_time - start_time) * 1000

print("Total sum: " + str(total))
print("Time taken: " + str(elapsed_ms) + " ms")
```

```
Total sum: 499999999500000000
Time taken: 91044.24285888672 ms
```

### Filmstock

```js
var s = 0

for var i = 1 ; i < 1000000000 ; it(i) {
    s += i
}

printNum(s)
```

```
499999999067108992.00

[Program finished in 3249ms with exit code: 0]
```

| Language | Optimization / Runtime | Time (ms) |
|---|---|---|
| Java | Default JVM (JIT Optimized (cheating)) | 777 |
| C | GCC -O3 but volatile sum | 2590 |
| Filmstock | Filmstock VM | 3249 |
| Java | -Xint (Interpreter Only) | 13874 |
| Python | CPython JIT (was a little faster than CPython) | 91044 |

## Planned features

- Rust compiler
