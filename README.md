# OpenTok Linux SDK Samples

<img src="https://assets.tokbox.com/img/vonage/Vonage_VideoAPI_black.svg" height="48px" alt="Tokbox is now known as Vonage" />

[![Build Status](https://travis-ci.org/opentok/opentok-linux-sdk-samples.svg?branch=master)](https://travis-ci.org/opentok/opentok-linux-sdk-samples)

This repository is meant to provide some examples for you to better understand
the features of the OpenTok Linux SDK.

The sample applications here can be used either in regular desktop environments
or in any tiny single-board computer such as the Raspberry Pi.

The sample applications are meant to be used with the latest version of the
OpenTok Linux SDK. Feel free to copy and modify the source code herein for your
own projects.

Please consider sharing your modifications with us, especially if they might
benefit other developers using the OpenTok Linux SDK. See the License for more
information.

## What's inside

### [Basic Video Chat](Basic-Video-Chat)

This sample application shows you how to connect to an OpenTok session, publish
a stream, and subscribe to a stream. It implements a simple video call
application with several clients. If you're just getting started with OpenTok,
this is where you should start.

### [Publisher Only](Publisher-Only)

This sample application shows how to publish an audio-video stream to a
session. This sample application might cover a suitable use case where you just
want to publish an audio-video stream to an OpenTok session, without subscribing to
streams from other clients connected to the session. Examples are a video
surveillance equipment application, a smart bell, etc.

### [Signaling](Signaling)

This sample application shows how to use the signaling API. This sample listens
for signals and echoes back whatever is sent to it.

### [Configurable TURN Servers](Configurable-TURN-Servers)

This sample application shows how to use the Configurable TURN servers API. This
sample uses user-provided TURN servers for the session.

### [Custom Audio Device](Custom-Audio-Device)

This sample application shows how to use the audio device API to implement a
custom audio capturer. As with the custom video driver, most applications can
use the default audio capturer. If you want to add custom audio manipulation, look
here.

### [Custom Video Capturer](Custom-Video-Capturer)

This sample application shows how to use both a custom video capturer and
renderer. While most applications will work fine with the default capturer
and renderer, so you won't need to implement a custom video driver. However,
if you need to add custom effects, then this is where you should start.

## Development and Contributing

Interested in contributing? We :heart: pull requests! See the
[Contribution](CONTRIBUTING.md) guidelines.

## Getting Help

We love to hear from you so if you have questions, comments or find a bug in the project, let us know! You can either:

- Open an issue on this repository.
- See <https://support.tokbox.com/> for support options.
- Tweet at us! We're [@VonageDev](https://twitter.com/VonageDev) on Twitter.
- Or [join the Vonage Developer Community Slack](https://developer.nexmo.com/community/slack).

## Further Reading

- Check out the Developer Documentation at <https://tokbox.com/developer/>.
