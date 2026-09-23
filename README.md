# mo

`mo` is a small C++23 reader for GNU gettext `.mo` catalogues.  It maps a
catalogue directly into memory and lets an application look up or iterate over
translations without copying the catalogue or constructing strings.

It is intended for applications that already have a compiled `.mo` file and
want a lean, read-only way to access it.

## Highlights

- **Memory-mapped.** The catalogue is read in place with
  [`mio::mmap_source`](https://github.com/vimpunk/mio); translations are views
  into the mapped file, not owned buffers.
- **No allocations after mapping.** Reading header fields, looking up an
  original string, retrieving a translation, and iterating entries perform no
  dynamic allocations.
- **Fast indexed lookup.** Lookups use the hash table stored in a GNU `.mo`
  file and its double-hashing probe sequence. This is more direct than a
  binary search over the original-string table, and does not require building
  an index at runtime.
- **Context-aware.** `msgctxt` entries can be queried using a separate context
  and original string.
- **Zero-copy strings.** Results are `zstring_view`s: `std::string_view`-like
  views that also guarantee a terminating NUL byte, so they can be passed to
  APIs requiring `const char*` with `c_str()`.

The `Mo` instance (which owns the memory map) must outlive every view obtained
from it.

## Requirements

- A C++23 compiler
- [mio](https://github.com/vimpunk/mio) (the project declares it in
  `vcpkg.json`)
- A GNU `.mo` file using format revision 0 and containing its hash table

Only the native, little-endian GNU MO magic number (`0x950412de`) is currently
accepted. The constructor rejects other magic numbers, format revisions, and
catalogues without a usable hash table.

## Usage

Map the file once, then construct `Mo` from the map:

```cpp
#include <mio/mmap.hpp>
#include <y/mo.h>
#include <utility>

auto file = mio::mmap_source{"locale/fr/LC_MESSAGES/app.mo"};
const y::mo::Mo catalogue{std::move(file)};

if (const auto index = catalogue.find_original_index("Save"); index >= 0) {
    const auto translated = catalogue.translation_string(index);
    // `translated` is a zero-copy zstring_view.
    display(translated.c_str());
}
```

`find_original_index()` returns `-1` when there is no matching entry. Use the
index with `original_string()` or `translation_string()` to retrieve either
side of the catalogue entry:

```cpp
const auto index = catalogue.find_original_index("File");
if (index >= 0) {
    assert(catalogue.original_string(index) == "File");
    std::cout << catalogue.translation_string(index) << '\n';
}
```

For messages with a gettext context, pass it separately:

```cpp
const auto index = catalogue.find_original_index("menu", "Open");
if (index >= 0) {
    use_label(catalogue.translation_string(index).c_str());
}
```

You can also enumerate the catalogue without allocating:

```cpp
for (const auto entry : catalogue) {
    std::cout << entry.original() << " => " << entry.translation() << '\n';
}
```

## Deliberately not supported: plural messages

This library does not interpret gettext plural entries. Supporting them would
mean reading and parsing the `Plural-Forms` text in the catalogue header in
order to evaluate its expression. That adds parser state and work to an API
whose central property is direct, allocation-free access to a mapped catalogue.

More importantly, gettext's plural-expression model is a limited fit for
modern localization. ICU MessageFormat-style systems can express richer
selection rules, including ordinal categories and gender-based selection, in
addition to cardinal plurals. Rather than implement and endorse a narrow
plural evaluator here, `mo` intentionally exposes only singular, context-aware
message lookup.

## Notes

`Mo` is a low-level reader, not a full gettext runtime. It does not select a
locale, provide fallback catalogues, parse headers beyond the file-format
checks, or transform encodings. Keep the `Mo` instance alive while using any
returned views.
