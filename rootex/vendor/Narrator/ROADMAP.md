# Roadmap

## Epics

- More documented and commented code
- Performance optimization and refactoring
- Remain features support and solve known limitations

## Unsupported features

- [ ] Tunnels
- [ ] Threads
- [ ] Divert targets as variable's type
- [ ] Assigning string evaluations to variables
- [ ] Knots and stitches as internal functions (take parameters and return values)


## Known limitations

- [ ] Choice's title can't contain inline conditions or alternatives
- [ ] Choice can't have few conditions like ```* { a } { b }```. *The solution is using ```* { a && b } ``` instead.*
- [ ] There is no query functions ```TURNS()``` and ```TURNS_SINCE()```
- [ ] A list uses only standard numerical values ```1, 2, 3...```. Can't define your own numerical values like ```4, 7, 12...```.
- [ ] A comment in the middle of the paragraph ```before /* comment */ and after``` splites it into two paragrphs ```before``` and ```and after```

## Ideas

- [ ] **Optimization**. Make lists a class types instead of dynamically attaching metatables? 
- [ ] **Optimization**. What about custom short keys in the book? For example, text = t, condition = q, success = y, failure = n?
- [ ] **Parser.** May be capture all the white spaces and then trim them and remove double spaces in paragraphs in the final output? The parser trims most of the white spaces at the moment. 
- [ ] **Parser.** Don’t divide expression and divert to two a book items, for example: “{ | { exp } -> divert }”
- [ ] **Style.** Snake case vs camel case?
