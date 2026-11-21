# Screen camera

See the screen's own camera in Home Assistant. The screen becomes a camera of its device there: a picture on a
dashboard, a stream when you open it, and anything Home Assistant does with a camera (a snapshot in an automation, a
recording).

The screen encodes snapshots and video in hardware. Tessera always shows a dot in the top right corner during camera use.

## What you need

- A Waveshare ESP32-P4-WIFI6-Touch-LCD-7 (720 × 1280) with its OV5647 camera and Tessera's `wavesharep47` board profile.

## Set up

1. In Tessera, open **Plugins** and choose **Add with a link**. Add this plugin's GitHub link, choose **Test a branch**
   and enter `lcd7-test`. Select the LCD-7 to build it with **Screen camera**.
2. Follow the [Video](#video) setup for live video and sound. If you switch **Video** off before building, the
   screen's ESPHome device in Home Assistant gets a **Camera** entity for snapshots. You can add it to a dashboard
   with a Picture entity or Picture glance card.
3. If the image is upside down, turn on **Camera upside down** in the plugin's settings.
4. Optional: switch on **Light level and motion** in the plugin's settings (the screen's Plugins tab in Tessera, or
   **Camera light level and motion** on the device in Home Assistant). Then **Light level** and **Motion** follow the
   room.

## Video

**Video** is on by default. It streams H.264 from the screen's hardware encoder and Opus audio from its microphone.

1. Build the screen with **Video** on in the screen's **Plugins** tab.
2. In Tessera, open **Settings → Video in Home Assistant**, select **Install in Home Assistant**, then restart Home
   Assistant. This is a one-time setup for all your Tessera screens.
3. Open the screen's device in Home Assistant and select **Video**. For live video on your dashboard, use that entity
   in a Picture entity card with `camera_view: live`. Tessera derives the video password from the screen's API key.

With the part, the camera runs from the screen's start, and its pictures come from the video: the camera **Camera**
leaves the screen's device, and **Video** gives the stills. Light level and motion keep working. The dot is solid while
someone watches, and the microphone listens only then. The video needs an ESP32-P4 with a microphone; it was tried on
the reTerminal D1001 and the Guition 10.1-inch.

On the Guition screens the setting **Camera field of view** chooses how much of the room the camera shows: **Full**,
the default, all it sees at half the resolution and 15 frames a second; **Sharp** the middle of it at full resolution.

Tessera's docs/CAMERA.md, "The screen's own camera as video in Home Assistant", says how it works and what it takes.

## Light level and motion

Sensing is off by default. Turn on **Camera light level and motion** to report both values in Home Assistant even
when nobody is watching the camera.

- **Light level**, an estimate in lux from the picture's brightness, the exposure time and the sensor's gain. It is right
  in proportion (twice the light reads about twice the lux), not exact: pick the thresholds of your automations by
  looking at what it reads in your room. It updates every ten seconds, and at once when a light goes on or off.
- **Motion**, from the picture compared in a coarse grid a few times a second; on when part of it changes, and for
  **Motion time-out** after (ten seconds to start with, five seconds to ten minutes). A light switched on or off (most of the picture brighter or darker at once) is not motion: for three
  seconds after it, while the camera adjusts, motion is not judged.

No picture is encoded or sent for them: only the two values leave the screen. The dot in the top right corner is a ring
while it runs, and solid once someone watches. While nobody watches, the camera makes 5 pictures a second, enough for
both, and goes back to full speed at once for the video or a picture. With **Motion wakes the screen** on (off to start
with), someone moving in front of the screen wakes it, as a touch does.

## Privacy

Nothing leaves the screen until Home Assistant asks for a picture or the video, and every picture goes only to your
Home Assistant. The video asks for a password made from the screen's own API key.
With light level and motion on, the camera looks all the time, but what it sees stays on the screen.
Everyone who can open the screen's device or a dashboard with its camera can see through it. To stop that for a while,
disable the camera entity in Home Assistant; to stop it for good, remove the plugin from the screen.

## For makers

The camera is ESPHome's camera with id `ts_camera` (the feature `camera`), so another plugin can build on it. The
camera driver is `esp_video_camera` (github.com/n-IA-hane/esphome-esp-video-camera, from ESPHome pull request
esphome/esphome#16944), copied into `components/esp_video_camera` under the ESPHome License, with its NOTICE; Tessera's few changes are marked
"Tessera" in it. Its empty esp_h264 for a JPEG-only build is a component of its own, `components/esp_h264_placeholder`,
which the part Video removes, so that the app starts the video's build from an empty build folder (here, and on a build server) and it finds the real encoder.
The camera's raw frames go to one consumer of the camera, Screen camera's frame hub (`components/screen_camera/frame_hub.h`),
which hands each to the light level and motion and to the video while they are on; a consumer reads a frame through
`frame_reader.h` (`tests/test_frame_reader.cpp` checks it on a PC). The sensors the camera finds by itself, and the
SC202CS's colour tuning, are the table in `components/screen_camera/sensors.py`: a sensor Espressif's camera library
drives is a row there. What a sensor's driver can do beyond its format, a slower frame rate while the camera only senses
and a mode over its whole field of view, it tells the camera through `components/esp_video_camera/camera_sensor.h`,
whichever board it is on. The video is `components/screen_video`: the frame path and Espressif's esp_h264 in
`video_encoder.cpp`, the RTSP server in `rtsp_server.cpp`, the microphone as Opus in `opus_sound.cpp`.
The OmniVision OV02C10 of the Guition screens, which Espressif's camera library has no driver for, is a component of its
own beside it, `components/ov02c10`: the sensor driver and its image tuning (one for rev3 chips, one for older ones),
with a NOTICE that says where they come from. The SmartSens SC202CS (SC2356) of the Tab5 and the reTerminal has a small
component of its own too, `components/sc202cs`: Espressif's driver starts it with a black level of 16 of 255 for the ISP
to take off, which an ESP32-P4 before rev3 cannot, so the component lowers the sensor's own black level target after the
camera has started it. The OmniVision OV5647 (the Waveshare ESP32-P4 7B's camera kit, the Raspberry Pi cameras of that
sensor) has one as well, `components/ov5647`: Espressif's formats set one of its two mirror bits and its flips move its
colours, so the component sets the other bit and moves the sensor's window by a pixel for each flip, every time the
camera has handed the sensor its controls; without it the picture was purple, with orange as green, and turned half way
round red and blue swapped. The component also reads the sensor's exposure and gain registers for light level and motion.
