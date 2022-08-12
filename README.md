## Direct YAML
Direct YAML is a lightweight, fast, single header YAML C++ parser.
- [How it works](#how-it-works)
- [Features](#features)
- [Installation](#installation)
- [Notes](#notes)
- [Example Reader](#example-reader)
- [License](#license)

## Usage
### load and parse rifle.yaml
```cpp
string memFile;
loadFromFile("../ymls/rifle.yaml", memFile);

// ctor
//Directyaml dym(&memFile[0]);

// or load with .parse
Directyaml dym;
dym.parse(&memFile[0]);

```

### load simple values
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

### load simple array
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

### load struct array
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
The idea is to parse the entire YAML file as hierarchical key-value pairs (heavily relies on the indentation of each line). A well formed YAML file is always important.

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

## Features
* MIT licensed
* No dependencies
  * std::vector and std::string are used.
  * std::string is only used as a container for managed parser.
* Fast
  * The source yaml is split and scanned (line-by-line) only once.
  * Even faster for well formed yaml (so checkIndents() will do nothing).
  * Even faster if no line-end-comments (undefine `__DYML_ALLOW_LINE_END_COMMENT__` to achieve this if guaranteed source yaml contains no line-end-comments)  
* Low memory usage
  * unmanaged: two std::vectors (one holds key-value (two pointers) + level(int) for each valid line, another holds top-levels (int))
  * managed: unmanaged + cached source yaml string
* Easy to use.
* Single header file library, tested on Android, iOS and Windows.
* Small codebase (< 450 loc)
* Destructive, your source yaml will be **modified**! (unmanaged parser, managed parser will modify its cached yaml internally)

## Installation
1. Download latest dyml.h
2. Include dyml.h in your project
    - Define `__DYML_IMPLEMENTATION__` before you include this file in a cpp file to create the implementation.
    ```cpp
    #define __DYML_IMPLEMENTATION__
    #include "dyml.h"
    ```
3. Compile with C++11 (or later) support (auto, std::vector.emplace_back)

## Notes
Direct YAML is a line-by-line parser, so 
* YAML version is not important
* `&` and `*` (YAML reference) are not supported  
* Don't split scalar into multiple lines like this, they won't crash but the parsing result may be not what you want:
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
The core of Diret YAML is the parser, once the parsing is done, what to do with the parsing result is totally up to you. Here is an example class to show you how to load values from the parsing result:
```cpp
// example 1
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
// example 2
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
Distributed under the MIT license. 