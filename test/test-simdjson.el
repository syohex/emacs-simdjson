;;; test-simdjson.el --- Test for simdjson.el

;; Copyright (C) 2020 by Shohei YOSHIDA

;; Author: Shohei YOSHIDA <syohex@gmail.com>

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:

;;; Code:

(require 'ert)
(require 'simdjson)
(require 'cl-lib)
(require 'subr-x)

(ert-deftest parse ()
  "Parse string as JSON"
  (let ((got (simdjson-parse "
{
  \"array\": [7, 8, 9],
  \"num\": 42,
  \"bool\": true,
  \"null\": null
}
")))
    (should (cl-typep got 'hash-table))
    (should (= (length (hash-table-keys got)) 4))
    (should (vectorp (gethash "array" got)))
    (should (= (aref (gethash "array" got) 2) 9))
    (should (= (gethash "num" got) 42))
    (should (eq (gethash "bool" got) t))
    (should (null (gethash "null" got)))))

;;; test-simdjson.el ends here
