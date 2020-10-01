# simdjson.el

[simdjson](https://github.com/simdjson/simdjson) binding for Emacs Lisp.

## Requirements

- C++ compiler(supports C++ 17)
- Emacs 27 or higher(configured with `--with-modules`)

## Interfaces

#### `(simdjson-parse string)`

Parse JSON `string` and return Emacs Lisp objects.

## Conversion Rule

| JSON                     | Emacs Lisp   |
|:-------------------------|:-------------|
| 123                      | integer      |
| 123.4                    | float        |
| "123"                    | string       |
| t, On, Yes, TRUE         | t : symbol   |
| n, No, Off, False        | nil : symbol |
| - aaa<br /> - bbb        | vector       |
| name: Foo<br /> year: 19 | hash-table   |

## License

- This package GPL3
- simdjson Apache 2.0 License
