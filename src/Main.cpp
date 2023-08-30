#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <jack/jack.h>
#include <jack/midiport.h>


jack_port_t *controllerInputPort;
jack_port_t *deviceInputPort;
jack_port_t *deviceOutputPort;
jack_client_t *client;


int process (jack_nframes_t nframes, void *arg)
{
    // Clear the output each process cycle.
    jack_midi_clear_buffer(deviceOutputPort);

    // You must get the buffer each cycle.
    void* deviceInputPortBuffer = jack_port_get_buffer(deviceInputPort, nframes);
    
    // Get number of events, and process them.
    jack_nframes_t inputEventCount = jack_midi_get_event_count(deviceInputPortBuffer);
    if(inputEventCount > 0)
    {
        for (jack_nframes_t i = 0; i < inputEventCount; i++)
        {
            jack_midi_event_t* event;
            
            int success = jack_midi_event_get(event, deviceInputPortBuffer, i);
            if (success == 0)
            {
                if (event->size == 1)
                {
                    // Ignore 0xF8 - seems to be a keep-alive from the Triton.
                    if (event->buffer[0] != 0xF8)
                    {
                        printf("Midi Event: BYTES %lu [%0X]\n", event->size, event->buffer[0]);
                    }
                }
                else if (event->size == 2)
                {
                    printf("Midi Event: BYTES %lu [%0X][%0X]\n", event->size, event->buffer[0], event->buffer[1]);
                }
                else if (event->size == 3)
                {
                    // printf("Midi Event: BYTES %lu [%0X][%0X][%0X]\n", event->size, event->buffer[0], event->buffer[1], event->buffer[2]);
                }
                else
                {
                    printf("Midi Event: BYTES %lu [%0X][%0X][%0X]...\n", event->size, event->buffer[0], event->buffer[1], event->buffer[2]);
                }

                // Let's examine the first byte to determine the message type.
                jack_midi_data_t firstByte = event->buffer[0];
                jack_midi_data_t channel = firstByte & 0x0F;
                jack_midi_data_t messageType = (firstByte >> 4) & 0x07;

                switch (messageType)
                {
                    case 0:
                        printf("Note Off:        Chan: [%0X]    Data: [%0X][%0X]\n", channel, event->buffer[1], event->buffer[2]);
                        break;

                    case 1:
                        printf("Note On:         Chan: [%0X]    Data: [%0X][%0X]\n", channel, event->buffer[1], event->buffer[2]);
                        break;

                    case 2:
                        printf("P. Aftertouch:   Chan: [%0X]    Data: [%0X][%0X]\n", channel, event->buffer[1], event->buffer[2]);
                        break;

                    case 3:
                        printf("Control Change:  Chan: [%0X]    Data: [%0X][%0X]\n", channel, event->buffer[1], event->buffer[2]);
                        break;

                    case 4:
                        printf("Program Change:  Chan: [%0X]    Data: [%0X][%0X]\n", channel, event->buffer[1], event->buffer[2]);
                        break;

                    case 5:
                        printf("C. Aftertouch:   Chan: [%0X]    Data: [%0X][%0X]\n", channel, event->buffer[1], event->buffer[2]);
                        break;

                    case 6:
                        printf("Pitch Wheel:     Chan: [%0X]    Data: [%0X][%0X]\n", channel, event->buffer[1], event->buffer[2]);
                        break;
                }

            }
            else
            {
                printf("Midi Event Failure! %i\n", success);
            }
        }
    }

    return 0;
}


void jack_shutdown(void *arg)
{
    exit(1);
}


// NOTE: Need far more robust releasing of resources in the case of failure to init.

int main (int argc, char* argv[])
{
    jack_status_t status;
    jack_options_t options {JackNullOption};   
    const char **ports;

    printf("Jack Test:\n");

    client = jack_client_open("JackTest", options, &status);
    if (client == NULL)
    {
        fprintf(stderr, "jack_client_open() failed. status = 0x%2.0x\n", status);
        if (status & JackServerFailed)
        {
            fprintf(stderr, "unable to connect to JACK server.\n");
        }
        exit(1);
    }

    if (status)
    {
        if (JackServerStarted) 
            fprintf(stderr, "JACK server started.\n");
      
        if (JackNameNotUnique)
            fprintf(stderr, "unique name '%s' assigned.\n", jack_get_client_name(client));
    }

    // // Register the controller input MIDI port.
    // controllerInputPort = jack_port_register(client, "ControllerInput", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    // if (deviceInputPort == NULL)
    // {
    //     fprintf(stderr, "no more JACK input port available.\n");
    //     exit(1);
    // }

    // Register a device input MIDI port.
    deviceInputPort = jack_port_register(client, "DeviceInput", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    if (deviceInputPort == NULL)
    {
        fprintf(stderr, "no more JACK input port available.\n");
        exit(1);
    }

    // Register a device output MIDI port.
    deviceOutputPort = jack_port_register(client, "DeviceOutput", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    if (deviceOutputPort == NULL)
    {
        fprintf(stderr, "no more JACK output port available.\n");
        exit(1);
    }
    
    // Set the callback for the main process.
    jack_set_process_callback(client, process, 0);
    
    // Set the callback for the shutdown process.
    jack_on_shutdown(client, jack_shutdown, 0);

    // Tell the server we're ready to process.
    if (jack_activate(client))
    {
        fprintf(stderr, "cannot activate client");
        exit(0);
    }
    
    // Get all the ports we opened.
    ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsOutput);
    
    if (ports == NULL)
    {
        fprintf(stderr, "no physical capture ports\n");
        exit(1);
    }
    
    // if (jack_connect(client, ports[0], jack_port_name(input_port)))
    // {
    //     fprintf(stderr, "cannot connect input ports\n");
    // }
    
    // Wait until the '.' key is pressed. Keeping the client open for a while.
    while (getchar() != '.')
    {
    }

    // We need to close all the ports by hand.
    jack_free(ports);

    // We're finished using Jack. Tell it to cleanup and close down.
    jack_client_close(client);

    return 0;
}