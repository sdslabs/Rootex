# Rootex Contribition Guidelines

This guide assumes you want to contribute on Rootex and have working knowledge of multiparadigm languages like C++. 

## Development

### When you are on a developmental task, start by skimming through these steps:

1. Decide where in the codebase you need to work. Read our physical design document for exact details. 
2. Discuss your task with team on how it should be done
3. Pick up a design choice for your task.
4. If it is a class - Decide who can use it, how many times this will be instantiated, how the public interface should look like, does it need a public interface? (E.g. IRenderingDevice is an interface for RenderingDeviceD3D), who can own objects of this class, who can delete objects of this class?
6. Next, ask whether it needs to be a class?, Is it a singleton or a static function API? Or just a single function or a macro?
7. Most of these come natually with experience but these points will help a lot in create a cleaner design
8. Don't overengineer. Don't overoptimize. We'd rather have code that runs like a slug rather than code that doesn't work.
9. Begin with writing the header files always
10. End with documentation and testing after code reviews

### C++ Style Guide
* Use clang-formatting tools in your IDE/text-editor for automated code consistency
* Nomenclature
```cpp
#include "include/path_file.h"

#define MACRO_CONSTANT 1.0

int globalVariable;
void globalFunction() {}

class PascalCase
{
public:
    static int s_PascalCase;
    int m_PascalCase;
    float camelCase(int argumentVariable);
    static int PascalCase();
};

enum class PascalCase
{
    ENUM_VALUE_1,
    ENUM_VALUE_2
};
```
### Git Commit Messages
* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 50 characters or less
* Reference issues and pull requests liberally after the first line by prefixing the issue number with `#`.
> E.g. Fix cases when events were not being handled #3
_where #3 is an issue number_

## Maintenance

### Opening an Issue
* Bug - Link the exact line where you found the bug or a way to reproduce the bug
* Crash Bug - Provide the stack trace at the point where the crash happens
* Convention Violation - Nomenclature inconsistency, class design inconsistency, unnecessary includes in headers
* Maintenance - Issues related to maintenance practices
* Proposal - State at least these 3 exact things in your proposal: Usage of feature, frequency of usage, and which part of codebase it goes in (helps in reviewing)

### Opening a PR
* 1 PR solves 1 issue (Not true in initial steps of any system but this will be valid eventually)
* Github will tag the exact directories where you have made a change. Verify those
* PRs should be reviewed in PR meetings only
