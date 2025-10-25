# CLI Search Tool

An incredibly fast and flexible command-line tool for searching text in files, filenames, and folder names.

## Installation

### From Releases

Download the latest pre-compiled binary from the [Releases](https://github.com/Lucop1911/CLI-search-tool/releases) page.

**Linux/macOS:**
```bash
chmod +x srctool
sudo mv srctool /usr/local/bin/
```

**Windows:**
Download `srctool.exe` and add it to your PATH.

### Build from Source

**Prerequisites:**
- GCC or Clang compiler
- Make (optional)

**Compilation:**
```bash
gcc -o srctool srctool.c
```

Or on Windows with MinGW:
```bash
gcc -o srctool.exe srctool.c
```

## Usage

```
srctool [OPTIONS] <search_term> [path]
```

### Options

| Flag | Description |
|------|-------------|
| `-c` | Search in file contents (default) |
| `-f` | Search in filenames |
| `-d` | Search in directory/folder names |
| `-a` | Search in all (contents, filenames, and folders) |
| `-i` | Case-insensitive search |
| `-r` | Recursive search through subdirectories |
| `-h` | Show help message |

### Examples

**Search for "hello world" in file contents (current directory):**
```bash
srctool -c "hello world" .
```

**Search for "main" in filenames recursively:**
```bash
srctool -f -r main /path/to/directory
```

**Search everywhere for "test" (case-insensitive, recursive):**
```bash
srctool -a -i -r test .
```

**Search only in folder names:**
```bash
srctool -d config /home/user
```

**Search for "TODO" in all C files in current directory:**
```bash
srctool -c TODO .
```


## Default Behavior

- If no search mode flags (`-c`, `-f`, `-d`, `-a`) are specified, the tool defaults to searching file contents (`-c`)
- If no path is specified, the tool searches in the current directory (`.`)
- Case-sensitive search is enabled by default
- Non-recursive search is the default

## Platform Support

- **Linux**: Full support
- **macOS**: Full support
- **Windows**: Supported (compile with MinGW or MSVC)
- **BSD**: Should work (untested)