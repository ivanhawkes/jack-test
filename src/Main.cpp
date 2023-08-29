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

    // Copy every input frame to the output.
    jack_nframes_t inputEventCount = jack_midi_get_event_count(deviceInputPort);
    for (jack_nframes_t i = 0; i < inputEventCount; i++)
    {
        jack_midi_event_t* event;
        
        if (int success = jack_midi_event_get(event, deviceInputPort, i))
        {
           // TODO: Copy the event data from the input to the output.
            if (event->size > 0)
            {
                printf("Midi Event: BYTES %lu [%0X][%0X][%0X]\n", event->size, event->buffer[0], event->buffer[1], event->buffer[2]);
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