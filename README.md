## Direct YAML
A lightweight, high-speed, single-header YAML parser for C++11+ with zero dependencies. Designed for simplicity and performance.

- [How it works](#how-it-works)
- [Key Features](#key-features)
- [Installation & Setup](#installation--setup)
- [Important Notes](#important-notes)
- [Example Reader](#example-reader)
- [License](#license)

## Usage Examples
### Load and Parse
```cpp
string memFile;
loadFromFile("../ymls/rifle.yaml", memFile);

// use ctor: 
//Directyaml dym(&memFile[0]);

// or load with .parse
Directyaml dym;
dym.parse(&memFile[0]);

```

### Read Simple Values
```yaml
%YAML 1.1
---
GeneralSettings:
  PivotPosition: [0.0599999987, -0.100000001, 0.25]
  FieldOfView: 47
  FireAnimationSpeed: 1.10000002
  MuzzleOffset: [-0.779999971, 0, -0.0500000007]
...  
```
cpp
```cpp
// load like this:
auto fov = MyNode(dym.root()["GeneralSettings"]["FieldOfView"]).read<float>();
auto pivot = MyNode(dym.root()["GeneralSettings"]["PivotPosition"]).read<vec3>();

// or like this:
auto node = dym.root()["GeneralSettings"];
auto fov = MyNode(node["FieldOfView"]).read<float>();
auto pivot = MyNode(node["PivotPosition"]).read<vec3>();
```

### Read Simple Arrays
```yaml
%YAML 1.1
---
GeneralSettings:
  PivotPosition: [0.0599999987, -0.100000001, 0.25]
  FieldOfView: 47
  Colors:
    - Green
    - Purple
    - Dark gray
    - Blue    
...
```
cpp
```cpp
const char* colors[0x20];
auto node = dym.root()["GeneralSettings"]["Colors"];

auto noc = node.children();
for (int i = 0; i < noc; ++i)  colors[i] = node[i].val();  
```

### Parse Struct Arrays
```yaml
%YAML 1.1
---
CamForces:
  Equip:
    - Delay: 0.349999994
      Force: [1, 0.5, -0.699999988]
      Distribution: 7
  EmptyReloadLoop:
    - Delay: 0.600000024
      Force: [0.400000006, -0.400000006, 0]
      Distribution: 5
    - Delay: 0.899999976
      Force: [0, 0, 2]
      Distribution: 10
    - Delay: 2.5999999
      Force: [-0.800000012, 0.200000003, -1]
      Distribution: 5
...
```
cpp
```cpp
struct Force { float delay; vec3 force; float distribution; };

Force forces[0x10];

node = dym.root()["CamForces"]["EmptyReloadLoop"];

int noc = node.children();
for (int i = 0; i < (noc / 3); ++i)
{
    auto& f = forces[i];
    MyNode(node[i * 3 + 0]).value(f.delay);
    MyNode(node[i * 3 + 1]).value(f.force);
    MyNode(node[i * 3 + 2]).value(f.distribution);
}
```

## How it works
Direct YAML parses the entire file into hierarchical key-value pairs based on indentation levels. It transforms your YAML into an efficient, flat row-based table internally, allowing for extremely fast lookups and low memory overhead.

Input YAML
```yaml
%YAML 1.1
---
GeneralSettings:
  PivotPosition: [0.0599999987, -0.100000001, 0.25]
  FieldOfView: 47
  FireAnimationSpeed: 1.10000002
  MuzzleOffset: [-0.779999971, 0, -0.0500000007]
  Colors:
    - Green
    - Purple
    - Dark gray
    - Blue
CamForces:
  Equip:
    - Delay: 0.349999994
      Force: [1, 0.5, -0.699999988]
      Distribution: 7
  EmptyReloadLoop:
    - Delay: 0.600000024
      Force: [0.400000006, -0.400000006, 0]
      Distribution: 5
    - Delay: 0.899999976
      Force: [0, 0, 2]
      Distribution: 10
    - Delay: 2.5999999
      Force: [-0.800000012, 0.200000003, -1]
      Distribution: 5
    - Delay: 1.70000005
      Force: [-0.800000012, 0, 0]
      Distribution: 5
...
```
Print the result as rows after parsing:
```
  Row  Level/Depth             key | value
--------------------------------------------------------------------------------
   #0   lv= 0      GeneralSettings |
   #1   lv= 1        PivotPosition | [0.0599999987, -0.100000001, 0.25]
   #2   lv= 1          FieldOfView | 47
   #3   lv= 1   FireAnimationSpeed | 1.10000002
   #4   lv= 1         MuzzleOffset | [-0.779999971, 0, -0.0500000007]
   #5   lv= 1               Colors |
   #6   lv= 2                      | Green
   #7   lv= 2                      | Purple
   #8   lv= 2                      | Dark gray
   #9   lv= 2                      | Blue
  #10   lv= 0            CamForces |
  #11   lv= 1                Equip |
  #12   lv= 2                Delay | 0.349999994
  #13   lv= 2                Force | [1, 0.5, -0.699999988]
  #14   lv= 2         Distribution | 7
  #15   lv= 1      EmptyReloadLoop |
  #16   lv= 2                Delay | 0.600000024
  #17   lv= 2                Force | [0.400000006, -0.400000006, 0]
  #18   lv= 2         Distribution | 5
  #19   lv= 2                Delay | 0.899999976
  #20   lv= 2                Force | [0, 0, 2]
  #21   lv= 2         Distribution | 10
  #22   lv= 2                Delay | 2.5999999
  #23   lv= 2                Force | [-0.800000012, 0.200000003, -1]
  #24   lv= 2         Distribution | 5
  #25   lv= 2                Delay | 1.70000005
  #26   lv= 2                Force | [-0.800000012, 0, 0]
  #27   lv= 2         Distribution | 5
```
Print the result as key-value hierarchy:
```
GeneralSettings:
  PivotPosition: [0.0599999987, -0.100000001, 0.25]
  FieldOfView: 47
  FireAnimationSpeed: 1.10000002
  MuzzleOffset: [-0.779999971, 0, -0.0500000007]
  Colors:
    Green
    Purple
    Dark gray
    Blue
CamForces:
  Equip:
    Delay: 0.349999994
    Force: [1, 0.5, -0.699999988]
    Distribution: 7
  EmptyReloadLoop:
    Delay: 0.600000024
    Force: [0.400000006, -0.400000006, 0]
    Distribution: 5
    Delay: 0.899999976
    Force: [0, 0, 2]
    Distribution: 10
    Delay: 2.5999999
    Force: [-0.800000012, 0.200000003, -1]
    Distribution: 5
    Delay: 1.70000005
    Force: [-0.800000012, 0, 0]
    Distribution: 5
```

## Key Features
* **MIT Licensed**: Free for personal and commercial use.
* **Zero Dependencies**: Uses only standard `std::vector` and `std::string`.
* **Single-Header**: Just drop `dyml.h` into your project and go.
* **Small Codebase**: Less than 450 lines of code.
* **High Performance**:
    * Scans the source line-by-line exactly once.
    * Optimized for well-formed YAML (skips unnecessary indentation checks).
    * Optional speed boost: Undefine `__DYML_ALLOW_LINE_END_COMMENT__` for even faster parsing if your files don't use end-of-line comments.
* **Low Memory Footprint**: 
    * **Unmanaged**: Uses only two vectors (one for key-value pointers + depth, one for top-level indices).
    * **Managed**: Same as unmanaged, plus an internal cache of the source string.
* **Cross-Platform**: Tested and verified on **Android, iOS, and Windows**.

## Installation & Setup
1. Download the latest `dyml.h`
2. Include dyml.h in your project
    - Define `__DYML_IMPLEMENTATION__` before you include this file in a cpp file to create the implementation.
    ```cpp
    #define __DYML_IMPLEMENTATION__
    #include "dyml.h"
    ```
3. Include dyml.h normally in your other files (as you would a standard header).
4. Compile with C++11 or later support

## Important Notes
* **Optimized for small-to-medium YAML Files**: The parser uses a dual-vector design (one for all parsed lines, one for root indices).
* **Destructive Parsing**: For maximum performance, the parser modifies the input buffer by replacing delimiters with null terminators. If you use the **managed parser**, it will modify its internal cache instead of your original string.
* **Indentation Matters**: The parser relies strictly on consistent indentation to determine the hierarchy.
* **Line-Based Parsing**: Because the logic is line-by-line:
    * The **YAML version** does not affect parsing.
    * **YAML References** (`&` and `*`) are **not supported**.
    * **Multi-line scalars** are **not supported**. Do not split values across multiple lines (using `|`, `>`, or unquoted multi-line strings), as the results will be unpredictable.

```yaml
# don't do this
  ASCII Art
  --- |
  \//||\/||
  // || ||__

# and this
>
  Sammy Sosa completed another
  fine season with great stats.
  63 Home Runs
  0.288 Batting Average
  What a year!

# and this
plain:
  This unquoted scalar
  spans many lines.
quoted: "So does this
  quoted scalar.\n"

```

## Example Reader
Direct YAML provides raw string access. You can easily use a wrapper class to handle type conversions. Here are two common patterns:
```cpp
// Example 1: Reference-based loading
class MyNode
{
public:
  MyNode(Directyaml::Node node) : _node(node) {}
  ~MyNode() {}

public :  
  template <typename T>
  void value(T& v) const {}

  // impl.
  template <> void value(const char*& v) const { v = _node.val(); }
  template <> void value(int& v) const  { v = strtol(_node.val(), nullptr, 10);  }
  template <> void value(float& v) const { v = strtof(_node.val(), nullptr); }
  template <> void value(vec3& v) const { sscanf(_node.val(), "[%f,%f,%f]", &v.x, &v.y, &v.z); }

protected:
  Directyaml::Node _node;
};

```
Another exmaple:
```cpp
// Example 2: Return-based loading
class MyNode
{
public:
  MyNode(Directyaml::Node node) : _node(node) {}
  ~MyNode() {}

public:
  template <typename T>
  T read() const {}

  template <>
  float read() const  { return strtof(_node.val(), nullptr); }

  template <>
  vec3 read() const
  {
    vec3 v;
    sscanf(_node.val(), "[%f,%f,%f]", &v.x, &v.y, &v.z);
    return v;
  }

protected:
  Directyaml::Node _node;
};
```

## License
Distributed under the **MIT license**. 
