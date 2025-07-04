# MediaSonic

<a href="https://syndromatic.com"><img src="src/gfx/MediaSonic.png" alt="MediaSonic icon" width="100"/></a>

**MediaSonic** is the default music and media player for **SynOS**, designed from the ground up as a beautiful, tactile, and immersive alternative to modern media applications. Built with the spirit of the Atmo design language and a deep respect for human-centric interaction, MediaSonic aims to make listening to music and watching videos a deeply engaging and joyful experience.

Currently in very early alpha, MediaSonic is a work in progress and is being actively developed to reflect the core philosophy of Syndromatic: clarity, charm, and control.

---

## ✨ Features

- **Rich Media Playback**
  Native support for a wide range of audio (MP3, FLAC, WAV, AIFF, M4A) and video (MP4, MOV) formats.

- **Cover Flow-inspired Browsing** *(WIP)*
  A visually stunning, dimensional carousel to browse your music library by album art, bringing a tangible feel to your digital collection.

- **Skeuomorphic Atmo UI**
  A beautiful interface crafted with the principles of the Atmo design language. Rich textures, realistic lighting, and intuitive controls that honour the golden era of skeuomorphism.

- **Qt & KDE Frameworks Powered**
  Leveraging the power of Qt 5 for native performance and seamless integration with the SynOS desktop.

- **Lightweight & Fast**
  Engineered for minimal resource usage without compromising on a rich feature set, ensuring your media plays back smoothly.

- **The Heart of Media on SynOS**
  Intended as the de facto media player for SynOS, providing a deeply integrated and cohesive experience.

---

## 💾 Installation
Installation has been tested on SynOS Canora (Beta 1) and should work on all Debian/Ubuntu-based Linux distributions with a Qt 5 based desktop environment.

```bash
git clone https://github.com/phenom64/MediaSonic.git
cd MediaSonic
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
sudo make install
```

---

## 🚧 Project Status

    ⚠️ MediaSonic is currently in very early alpha.
    While the application compiles and basic playback is functional, many components such as full metadata integration, playlist management,
    and advanced view modes (like Cover Flow) are incomplete or experimental.
    Retina display support is also currently a work in progress.

Expect bugs, unfinished features, and rough edges. We welcome collaboration and testing, but this is not yet ready for daily use.

---

## 🧠 Philosophy & Goals
MediaSonic is more than just a media player. It's a statement against the sterile, flat, and ephemeral nature of modern software. We believe that interacting with your music and video library should be a personal and delightful experience, not just a utility. It's about bringing back the feeling of holding an album in your hands, the joy of a dedicated music machine, and the sense of ownership over your collection.

As a cornerstone of the SynOS Atmo design language, MediaSonic will continue to evolve into a full-featured, skeuomorphic, and deeply human media experience that respects both the art it plays and the person listening.
