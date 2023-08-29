#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <jack/jack.h>


jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;


int process (jack_nframes_t nframes, void *arg)
{
    jack_default_audio_sample_t *in, *out;
    
    in = reinterpret_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(input_port, nframes));
    out = reinterpret_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(output_port, nframes));
    memcpy(out, in, sizeof(jack_default_audio_sample_t) * nframes);
    
    return 0;
}


void jack_shutdown(void *arg)
{
    exit(1);
}


int main (int argc, char* argv[])
{
    jack_status_t status;
    jack_options_t options {JackNullOption};   

    printf("Jack Test\n");

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

    printf("Jack opened client\n");

    while (getchar() != '.')
    {
    }


    jack_client_close(client);

    return 0;
}