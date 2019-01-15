## Usage
Open a console and run:​

    decryptor <path/to/file>

Output is a newline delimited json file saved on the input file path, using the same name but with `_d` before extension. E.g. `20180808T175623_d.ndjson` for an input file `20180808T175623.ndjson`.
Each json structure contains packets received at that time.
```json

{
  "server":"192.170.58.126:26505",
  "packets":["48494A4B4C4D4E4F","..."],
  "source":"Client",
  "timestamp":"2018-Oct-15 14:50:37"
}

```

For more information about file format visit http://ndjson.org.

## Compilation
To build this project from sources, the following is required:
- Microsoft Visual C++ (15.0)
- CMake (3.8)
- Boost (1.68)
- Nlohmann/Json (3.3.0)
- LZO (2.10)

#### Using Visual Studio 2017
1. Open the project in Visual Studio using `File -> Open -> CMake`.
2. Select build config (Debug/Release).
3. Configure CMake Settings `CMake -> Change CMake Settings -> packet_decryptor`. Add the path to Boost, Nlohmann/Json and LZO as `CMAKE_PREFIX_PATH` in `cmakeCommandArgs`.
4. Build using `CMake -> Build All`.

#### Using console
Inside project folder run:

    mkdir build && cd build
    cmake -G "Visual Studio 15 2017" -DCMAKE_PREFIX_PATH="/path/to/boost;/path/to/nlohmann_json;path/to/lzo" ../
    cmake --build .