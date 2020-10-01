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

(benchmark-run-compiled 100
  (json-read-from-string twitter-json-string))

(benchmark-run-compiled 100
  (simdjson-parse twitter-json-string))
