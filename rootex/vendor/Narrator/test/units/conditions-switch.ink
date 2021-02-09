VAR foo = false
VAR x = 0

-> simple

== simple
{not foo:    
   Hello!     
   }
{ foo :
True! -> choice
- else:
False! }
~ foo = not foo
-> simple

=== choice

{ true :
    * [Answer] -> nested_inline
}

== nested_inline
{
    - foo : { foo: True! | False! }
    Text here.
    - else: Badaboom!
    ...again. -> nested_block
}
~ foo = not foo
-> nested_inline


== nested_block
I love you. { true:
    And what about you?
    {
        - false:
            False!
        - true:
            * Choice -> tags
    }
    ...
} Something.

== tags
{ 
- not foo:
    text { true : x = { x } } #success
- else: 
    -> not_a_label
}
~ foo = not foo
-> tags

== not_a_label
{
- foo: suc...
    success #success
- x < 0: mid...
    middle
    text -> END
- else: els...
    (notLabel) else
    -> switch
}
~ foo = not foo
-> not_a_label

== switch
{ x:
- 0: 	zero 
- 1: 	one 
- 2: 	two 
- else: lots -> END
}
~ x++
-> switch