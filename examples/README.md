# Examples

This directory contains examples of how to use remi.

Directories starting with `__` are not examples, but rather helper directories for the examples.

Each example directory must contain a `info.json` file with the following structure:

```ts
{
  name: string, // the name of the example
  description: string, // a short description of the example
  src_files: string[], // relative paths to src files
  assets_dir: string, // relative path to the assets directory or an empty string if there are no assets
}
```
