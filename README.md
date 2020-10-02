# simdjson.el

[simdjson](https://github.com/simdjson/simdjson) binding for Emacs Lisp.

## Requirements

- C++ compiler(supports C++ 17)
- Emacs 27 or higher(configured with `--with-modules`)

## Interfaces

#### `(simdjson-parse string)`

Parse JSON `string` and return Emacs Lisp objects.

## Benchmark

`json-read-from-string`(native support) vs `simdjson`

Test environment
- Intel Core i7-8565U 1.80GHz
- Memory 24GB
- Ubuntu 20.04
- GCC 9.3.0

```lisp
(require 'url)
(require 'url-http)
(require 'benchmark)
(require 'json)
(require 'simdjson)

(defconst twitter-json-url
  "https://raw.githubusercontent.com/simdjson/simdjson/master/jsonexamples/twitter.json")

(defun get-twitter-json ()
  (with-current-buffer (url-retrieve-synchronously twitter-json-url)
    (goto-char url-http-end-of-headers)
    (buffer-substring-no-properties
     (point) (point-max))))

(defvar twitter-json-string (get-twitter-json))

;; elisp
(benchmark-run-compiled 100
  (json-read-from-string twitter-json-string))
;; => (11.859395746999999 950 7.268225878)

;; jansson
(benchmark-run-compiled 100
  (json-parse-string twitter-json-string))
;; (2.1292452890000004 100 0.925590689999999)

;; This binding
(benchmark-run-compiled 100
  (simdjson-parse twitter-json-string))
;; => (7.14668513 740 6.646881686)
```

### Consideration

Converting parse result into Emacs lisp object is very slow.

- Parsing twitter.json time with simdjson 3096us
- Convert its result into Emacs Lisp Object 87561us


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
