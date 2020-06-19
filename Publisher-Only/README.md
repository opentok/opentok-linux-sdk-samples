# Publisher Only

The Publisher Only sample application is a very simple application meant to get
a new developer started using the OpenTok Linux SDK.

Upon building and running this sample application, you should be able to have
one way audio and video communication using OpenTok. This sample application is
intended to be used in a tiny single-board computer such as the Raspberry Pi.

Note that you will need a valid TokBox account for this app. Edit the
[main.cpp](main.cpp) file and add your API key, session id and token.

## Building and running the sample app

Let's build this sample for the Raspberry Pi device. For now the SDK build for
this device is distributed via a direct download in the TokBox developer center
as a `tgz` package.

Before building the sample application you have to download the OpenTok Linux SDK
package and uncompress it somewhere locally (e.g. [assets](../assets) folder
you create).

```
$ cd assets; wget https://tokbox.com/downloads/libopentok_linux_llvm_armv7-latest
```

Uncompress the one for the target host you are building the sample application
for (e.g. `armv7`).

```
$ cd assets; unp libopentok_linux_llvm_armv7-2.18.0.tgz
```

This will create a `libopentok_linux_llvm_armv7` folder at
[assets/libopentok_linux_llvm_armv7](../assets/libopentok_linux_llvm_armv7). The
build system will try find the OpenTok Linux SDK header files and library under
the `assets` folder.

The folder above contains a handy `CMakeListz.txt` file the developer can use if
`CMake` is used in the application. The developer would need to add it via the
`ADD_SUBDIRECTORY()` command, add `$<TARGET_PROPERTY:libopentok,INTERFACE_INCLUDE_DIRECTORIES>`
to the list of include folders with the `INCLUDE_DIRECTORIES()` command and
finally add `libopentok` to the list of target link libraries.

```
  ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../assets/libopentok_linux_llvm_armv7 ${CMAKE_CURRENT_BINARY_DIR}/libopentok)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../common/src ${LIBSDL2_INCLUDE_DIRS} $<TARGET_PROPERTY:libopentok,INTERFACE_INCLUDE_DIRECTORIES>)
  LINK_DIRECTORIES(${LIBSDL2_LIBRARY_DIRS})

  ADD_EXECUTABLE(${PROJECT_NAME} main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/renderer.cpp)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBSDL2_LIBRARIES} libopentok)
```

We use [CMake](https://cmake.org) in this sample application so you have to
install it before trying to build the sample application. There are other
dependencies you have to install too. These are:
  - build-essential
  - cmake
  - clang
  - libc++-dev
  - libc++abi-dev
  - pkg-config
  - libasound2
  - libpulse-dev
  - libsdl2-dev

Let's build the sample application. Since it's good practice to create a build
folder, let's go ahead and create it in the project directory.

```
$ mkdir Publisher-Only/build
```

Next step is to create the building bits using `cmake`.

```
$ cd Publisher-Only/build
$ CC=clang CXX=clang++ \
  cmake -DLIBOPENTOK_INCLUDE_DIRS=$(pwd)/../../assets/libopentok_linux_llvm_armv7/include \
        -DLIBOPENTOK_LIBRARY_DIRS=$(pwd)/../../assets/libopentok_linux_llvm_armv7/lib \
  ..
```

Note we are using `clang/clang++` compilers here.

We will use `make` to build the code.

```
$ make
```

Run the `publisher_only` binary once it's built.

```
$ ./publisher_only
```

You can end the sample application by using the Control + C combination in the console.
