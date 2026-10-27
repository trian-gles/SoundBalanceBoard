I'm creating a realtime balance board which controls a pure data patch

# Architecture
- ESP32 as microcontroller
- 4 Load Cells with HX71708-ADCs
- readout data at 320 Hz

# Goals
- find oout where most of the weight is on the board and calculate polar coordinates from this