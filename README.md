# MIDI-Receiver
An Arduino-based MIDI synthesizer.
[Demo!](https://youtu.be/fXnodor5SoA)

Makes a MIDI device out of anything that makes noise when current or voltage is applied.

For USB control, use [Hairless MIDI-Serial Bridge](https://github.com/projectgus/hairless-midiserial).
For the MIDI loopback that will probably be needed, use [MIDIOX's MIDI-Yoke] (http://www.midiox.com/index.htm?http://www.midiox.com/myoke.htm).

If you already have a MIDI connection to your computer, or you want to connect this Arduino straight to a MIDI source (e.g, Keyboard), then you probably know more about using MIDI than I do. In case you don't, though, pin 0 to upstream TX, pin 1 to upstream RX, both with a resistor inline.

# How does it work?

The Arduino tone function is used to drive pin 8, producing a square wave with a frequency matching the note commanded by the most recent 'note on' (0x8#) packet.

The program interprets a somewhat narrow section of the MIDI standard, but it should keep up with most playback.
