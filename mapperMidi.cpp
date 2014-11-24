#include <mapper/mapper_cpp.h>

#include <stdio.h>
#include <math.h>
#include <lo/lo.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <vector>

#include "RtMidi.h"

bool chooseMidiPort( RtMidiOut *rtmidi );

RtMidiOut *midiout;

int done;

void ctrlc(int signal)
{
    done = 1;
}

void input_handler_f ( mapper_signal msig,
                    mapper_db_signal props,
                    int instance_id,
                    void *value,
                    int count,
                    mapper_timetag_t *tt )
{
    int val = *(int*)value;
    printf("freq val = %i\n", val);
    std::vector<unsigned char> message;

    message.push_back(176);
    message.push_back(10);
    if (val == 1)
        message.push_back( 100 );
    else
        message.push_back( 0 ); //off
    
    midiout->sendMessage( &message );
}

void input_handler_w ( mapper_signal msig,
                    mapper_db_signal props,
                    int instance_id,
                    void *value,
                    int count,
                    mapper_timetag_t *tt )
{
    
    printf("received val2 = %i\n",*(int*)value);
}

int main(int argc, char** argv) {
    
    printf("opening MIDI port...\n");
    midiout = 0;
    

    
    // RtMidiOut constructor
    try {
        midiout = new RtMidiOut();
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
        exit( EXIT_FAILURE );
    }
    
    // Call function to select port.
    try {
        if ( chooseMidiPort( midiout ) == false ) done = true;
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
        done = true;
    }
    
    //test output
    std::vector<unsigned char> message;
    message.push_back(176);
    message.push_back(10);
    message.push_back(100);
    midiout->sendMessage( &message );
    sleep(1);
    message.clear();
    message.push_back(176);
    message.push_back(10);
    message.push_back(0);
    midiout->sendMessage( &message );

    
    signal(SIGINT, ctrlc);
    mapper::Device dev("MapperToMidi");
    printf("Creating mapper device MapperToMidi");
    
    while (!dev.ready()) {
        printf("waiting for device...\n");
        dev.poll(500);
    }
    
    printf("\n\nReady!\n");
    
    mapper::Signal sig_in1 = dev.add_input("/CC_10", 1, 'i', "0-127", 0, 0, input_handler_f, 0);
    mapper::Signal sig_in2 = dev.add_input("/CC_01", 1, 'i', "0-127", 0, 0, input_handler_w, 0);
    
    while (!done) {
        dev.poll(50);
    }
    printf("\n\ndone. quitting!\n");
    
    delete midiout;
    
    return 0;

    
}


bool chooseMidiPort( RtMidiOut *rtmidi )
{
    std::string portName;
    unsigned int i = 0, nPorts = rtmidi->getPortCount();
    if ( nPorts == 0 ) {
        std::cout << "No output ports available!" << std::endl;
        return false;
    }
    
    if ( nPorts == 1 ) {
        std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
    }
    else {
        for ( i=0; i<nPorts; i++ ) {
            portName = rtmidi->getPortName(i);
            std::cout << "  Output port #" << i << ": " << portName << '\n';
        }
        
        do {
            std::cout << "\nChoose a port number: ";
            std::cin >> i;
        } while ( i >= nPorts );
    }
    
    std::cout << "\n";
    rtmidi->openPort( i );
    
    return true;
}