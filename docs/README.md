# Rootex Documentation

[![Documentation Status](https://readthedocs.org/projects/rootex/badge/?version=latest)](https://rootex.readthedocs.io/en/latest/?badge=latest)

## Where is the documentation?

Hosted at: https://rootex.readthedocs.io/

## How to write the documentation?

Use normal Doxygen style comments in C++ code. Refer the [Doxygen Documentation page on comments](http://www.doxygen.nl/manual/docblocks.html) for more information.

In Rootex, we try to keep our documentation as short and brief as possible so that the Doxygen comments don't oppress what the code is saying. Don't say things that the code has already said.

We prefer adding `/// This is a some piece of documentation` style comments to our source code to keep the C++ files as small as possible.

## Where should I write the documentation?

Describe the API design and usage in the .h files: that's the public interface for users.

Describe the implementation alternatives/issues and decisions in the .cpp files: that's for yourself later and other maintainers/enhancers, even someone reviewing the design as input to some next-gen system years hence.

## How to build the documentation?

0. Pre-requisites: Doxygen

1. Navigate to this directory.

```powershell
cd docs/
```

2. Install Python3 dependencies.

```powershell
pip3 install -r requirements.txt
```

3. Start the documentation builder.

```powershell
.\make.bat html
```

4. Open the homepage in your browser.

```powershell
start build/html/index.html
```
