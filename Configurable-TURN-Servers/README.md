# Configurable TURN Servers

The Configurable TURN Servers sample application shows how to use your own
TURN servers to route media traffic. See
[this documentation](https://tokbox.com/developer/guides/configurable-turn-servers/)
for basic information on using your own TURN servers in an OpenTok application.

This sample builds upon the [Publisher Only](../Publisher-Only) sample, with
settings for using your own TURN servers.

You will need a valid [Vonage Video API](https://tokbox.com/developer/)
account to build this app. (Note that OpenTok is now the Vonage Video API.)

## Building and running the sample app

This sample app has you specify only one TURN server. However, the
OpenTok Linux SDK supports the use of multiple TURN servers.

This application uses [CMake](https://cmake.org). Before building
the sample application, install it and these other dependencies:

  - build-essential
  - cmake
  - clang
  - libc++-dev
  - libc++abi-dev
  - pkg-config
  - libasound2
  - libpulse-dev
  - libsdl2-dev

The OpenTok Linux SDK is available as a Debian package. We maintain our own
Debian repository on Bintray. Run the following commands to install packages
from it.

* Add a new entry to your `/etc/apt/sources.list` file.

```bash
echo "deb https://dl.bintray.com/tokbox/debian buster main" | sudo tee -a /etc/apt/sources.list
```

* Add Bintray's GPG Key.

```bash
wget -O- -q https://bintray.com/user/downloadSubjectPublicKey?username=bintray | sudo apt-key add -
```

* Resynchronize the package index files from their sources.

```bash
sudo apt-get update
```

* Install the OpenTok Linux SDK packages.

```bash
sudo apt-get install libopentok-dev
```

Once you have installed the dependencies, you can build the sample application.
Since it's good practice to create a build folder, let's go ahead and create it
in the project directory:

```
$ mkdir Configurable-TURN-Servers/build
```

Copy the [config-sample.h](../common/src/config-sample.h) file as `config.h` at
`Configurable-TURN-Servers/`:

```
$ cp common/src/config-sample.h  Configurable-TURN-Servers/
```

Edit the `config.h` file and add your OpenTok API key,
an OpenTok session ID, and token for that session. For test purposes,
you can obtain a session ID and token from the project page in your
[Vonage Video API](https://tokbox.com/developer/) account. However,
in a production application, you will need to dynamically obtain the session
ID and token from a web service that uses one of
the [Vonage Video API server SDKs](https://tokbox.com/developer/sdks/server/).

Edit the [main.cpp](main.cpp) file, replacing following with values
for your TURN server:

* `"<ICE_SERVER_URL>"` -- The URL of the TURN server.

* `"<ICE_SERVER_USER>"` -- The username for the TURN server.

* `"<ICE_SERVER_CREDENTIAL>"` -- The credential string for the TURN server.

Next, create the building bits using `cmake`:

```
$ cd Configurable-TURN-Servers/build
$ CC=clang CXX=clang++ cmake ..
```

Note we are using `clang/clang++` compilers.

Use `make` to build the code:

```
$ make
```

When the `configurable_turn_servers` binary is built, run it:

```
$ ./configurable_turn_servers
```

You can use the [OpenTok Playground](https://tokbox.com/developer/tools/playground/)
to connect to the OpenTok session in a web browser, view the stream published
by the Custom TURN Servers app, and publish a stream that the app can subscribe to.
After entering the session ID in the "Join existing session" tab of the first page
of the Playground, click the "Advanced Firewall Control" link (near the top of
the page) and enter the information for your custom TURN server. Then click
the Continue button and connect to the session (by clicking the Connect button).s

You can end the sample application by typing Control + C in the console.

## Understanding the code

This application uses the same concepts that
the [Basic Video Chat](../Basic-Video-Chat) application uses. See the
[Understanding the code section](../Basic-Video-Chat/README.md#understanding-the-code)
of that application's README file to see how to connect to a session, publish
a stream, and subscribe to a stream.

### Setting TURN server options

The application initializes an `ice_config` variable of type `otc_custom_ice_config`,
defined in the OpenTok Linux SDK:

```
struct otc_custom_ice_config ice_config;
ice_config.num_ice_servers = 1;
ice_config.ice_url = (char **)malloc(sizeof(char *) * 1);
ice_config.ice_url[0] = (char *)malloc(sizeof(char) * MAX_CHAR_ARRAY_LENGTH);
memset(ice_config.ice_url[0], '\0',  MAX_CHAR_ARRAY_LENGTH);
strcpy(ice_config.ice_url[0], "<ICE_SERVER_URL>");
ice_config.ice_user = (char **)malloc(sizeof(char *) * 1);
ice_config.ice_user[0] = (char *)malloc(sizeof(char) * MAX_CHAR_ARRAY_LENGTH);
memset(ice_config.ice_user[0], '\0',  MAX_CHAR_ARRAY_LENGTH);
strcpy(ice_config.ice_user[0], "<ICE_SERVER_USER>");
ice_config.ice_credential = (char **)malloc(sizeof(char *) * 1);
ice_config.ice_credential[0] = (char *)malloc(sizeof(char) * MAX_CHAR_ARRAY_LENGTH);
memset(ice_config.ice_credential[0], '\0',  MAX_CHAR_ARRAY_LENGTH);
strcpy(ice_config.ice_credential[0], "<ICE_SERVER_CREDENTIAL>");
ice_config.force_turn = OTC_TRUE;
ice_config.use_custom_turn_only = OTC_FALSE;
```

The type `otc_custom_ice_config` struct includes the following members:

* `num_ice_servers` -- The number of ICE servers

* `ice_url` -- An array of strings specifying your ICE server URLs.

* `ice_user` -- An array of strings specifying usernames for the TURN servers.

* `ice_credential` -- An array of strings specifying credential strings for
  the TURN servers.

The application instantiates an `otc_session_settings` struct,
defined in the OpenTok Linux SDK:

```c
otc_session_settings *session_settings = otc_session_settings_new();
```

Then the app then calls the `otc_session_settings_set_custom_ice_config` function,
to add the TURN server settings to the otc_session_settings instance:

```c
if (session_settings != nullptr) {
  otc_session_settings_set_custom_ice_config(session_settings,
                                             &ice_config);
}
```

Finally, the app instantiates the session using the `otc_session_new_with_settings()`
function, defined in the OpenTok Linux SDK:

```c
otc_session *session = nullptr;

session = otc_session_new_with_settings(API_KEY,
                                        SESSION_ID,
                                        &session_callbacks,
                                        session_settings);
```

Note that this function lets you specify advanced settings,
including custom TURN servers, when instantiating an `otc_session` struct.

The session will use the TURN server you specify to route media streams
used by all publishers and subscribers.

## Next steps

See the [Vonage Video API developer center](https://tokbox.com/developer/)
for more information on the OpenTok Linux SDK.
