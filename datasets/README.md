# Datasets
Coordinates of nodes and queries are saved in this folder.

## Filename format

- category (nodes or queries)
- platform
- dimensions
- number of elements
- minimum coordinate value
- maximum coordinate value

### Example

`nodes_x64_128_250_0_184.bin` means that:

- The file contains coordinates for nodes.
- It was created on a x64 platform.
- There are 128 dimensions.
- 250 nodes.
- There are no coordinates with value less than 0.
- And no coordinates with value more than 184.

## Content format

- number of coordinates (size_t)
- coordinates (array of floats)

> Warning! The size of datatypes in the file depends on the platform the dataset was created on.

### Example

The content of a file named `nodes_x64_128_250_0_184.bin` would look like this after decoding:

- 32000
- 1.234
- 5.678
- ...

Bear in mind that the data is written in binary mode.
