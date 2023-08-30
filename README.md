# Jack Audio Test Code

Just testing use of Jack Audio to send and recieve MIDI messages.

## Testing

Start a copy of QJackCtl. Make sure you have these settings to disable MIDI handling by Jack.

    * MIDI Driver - None
    * Enable ALSA Sequencer Support - False

Start up the a2j MIDI demon:

```
a2jmidid -e
```

Now start the test app. It should create it's own Jack entries. You can now wire those up to the ones provided by a2jmidid.