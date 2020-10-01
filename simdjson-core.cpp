/*
  Copyright (C) 2016 by Syohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cinttypes>
#include <cstddef>
#include <string>

#include <simdjson.h>

#include <emacs-module.h>

int plugin_is_GPL_compatible;

namespace {

emacs_value to_emacs_lisp_object(emacs_env *env, const simdjson::dom::element &element) {
    switch (element.type()) {
    case simdjson::dom::element_type::ARRAY: {
        std::vector<emacs_value> array;
        for (const auto &elem : element) {
            array.push_back(to_emacs_lisp_object(env, elem));
        }

        emacs_value Fvector = env->intern(env, "vector");
        return env->funcall(env, Fvector, array.size(), &array[0]);
    }
    case simdjson::dom::element_type::OBJECT: {
        emacs_value Fmake_hash_table = env->intern(env, "make-hash-table");
        emacs_value make_hash_args[] = {
            env->intern(env, ":test"),
            env->intern(env, "equal"),
        };
        emacs_value hash = env->funcall(env, Fmake_hash_table, 2, make_hash_args);
        emacs_value Fputhash = env->intern(env, "puthash");

        for (const auto &&field : simdjson::dom::object(element)) {
            std::string_view view(field.key);
            emacs_value key = env->make_string(env, view.data(), view.size());
            emacs_value value = to_emacs_lisp_object(env, field.value);
            emacs_value puthash_args[] = {key, value, hash};
            env->funcall(env, Fputhash, 3, puthash_args);
        }

        return hash;
    }
    case simdjson::dom::element_type::INT64:
        return env->make_integer(env, element.get_int64());
    case simdjson::dom::element_type::UINT64:
        return env->make_integer(env, element.get_uint64());
    case simdjson::dom::element_type::DOUBLE:
        return env->make_float(env, element.get_double());
    case simdjson::dom::element_type::STRING: {
        const std::string_view view(element);
        return env->make_string(env, view.data(), view.size());
    }
    case simdjson::dom::element_type::BOOL:
        return static_cast<bool>(element) ? env->intern(env, "t") : env->intern(env, "nil");
    case simdjson::dom::element_type::NULL_VALUE:
        return env->intern(env, "nil");
    default:
        return env->intern(env, "nil");
    }
}

std::string retrieve_string(emacs_env *env, emacs_value str) {
    ptrdiff_t size;
    env->copy_string_contents(env, str, nullptr, &size);

    std::string ret;
    ret.resize(size - 1);
    env->copy_string_contents(env, str, &ret[0], &size);
    return ret;
}

emacs_value Fsimdjson_core_parse(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) noexcept {
    std::string input = retrieve_string(env, args[0]);

    simdjson::dom::parser parser;
    simdjson::dom::element doc;
    simdjson::padded_string str(input);
    auto error = parser.parse(str).get(doc);
    if (error != simdjson::SUCCESS) {
        return env->intern(env, "nil");
    }

    return to_emacs_lisp_object(env, doc);
}

void bind_function(emacs_env *env, const char *name, emacs_value Sfun) {
    emacs_value Qfset = env->intern(env, "fset");
    emacs_value Qsym = env->intern(env, name);
    emacs_value args[] = {Qsym, Sfun};

    env->funcall(env, Qfset, 2, args);
}

void provide(emacs_env *env, const char *feature) {
    emacs_value Qfeat = env->intern(env, feature);
    emacs_value Qprovide = env->intern(env, "provide");
    emacs_value args[] = {Qfeat};

    env->funcall(env, Qprovide, 1, args);
}

} // namespace

extern "C" {

int emacs_module_init(struct emacs_runtime *ert) {
    emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) bind_function(env, lsym, env->make_function(env, amin, amax, csym, doc, data))

    DEFUN("simdjson-core-parse", Fsimdjson_core_parse, 1, 1, "parsing JSON string", nullptr);

#undef DEFUN

    provide(env, "simdjson-core");
    return 0;
}

} // extern c
