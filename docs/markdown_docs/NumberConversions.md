## Implicit number conversions in Regal

### Number types:
    int32: 32-bit integer
    int64: 64-bit integer
    float32: 32-bit floating-point value
    float64: 64-bit floating-point value

In Regal, operations are typechecked before runtime. 

However, number types are unique in that they implicitly combine without throwing a type error.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
e.g. `12 + 3.8 = 15.8` implicitly combines a `int32` with a `float32` and results in a `float64`.

Thus, number type combinations have the following precedence:

`A <> B` &rarr; `Result` means `A` type *combines* with `B` type and has `Result` type  
&nbsp;&nbsp;&nbsp;&nbsp;`float64 <> ANY` &rarr; `float64`

&nbsp;&nbsp;&nbsp;&nbsp;`int64 <> int64` &rarr; `int64`  
&nbsp;&nbsp;&nbsp;&nbsp;`int64 <> float32` &rarr; `float64`  
&nbsp;&nbsp;&nbsp;&nbsp;`int64 <> int32` &rarr; `int64`

&nbsp;&nbsp;&nbsp;&nbsp;`float32 <> float32` &rarr; `float64`  
&nbsp;&nbsp;&nbsp;&nbsp;`float32 <> int32` &rarr; `float64`

&nbsp;&nbsp;&nbsp;&nbsp;`int32 <> int32` &rarr; `float64`

Specific combinations, *division* (`/`) and *exponentitation* (`**`), always result in a `float64` since the integers  
are not closed under these operations.

In a combination that happens before runtime (a constant combination), the result is then **wrapped**.  
This means the result of the combination is placed in the smallest type that can hold it.

For combinations that happen at runtime, no wrapping occurs, leading to faster execution but suboptimal memory use.