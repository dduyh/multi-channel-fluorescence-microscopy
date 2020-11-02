# multi-channel-fluorescence-microscopy
Codes for a self-built  Multi-channel Fluorescence Microscopy with designed Low-cost Iillumination Module.
Command lines for Arduino to control the two illumination systems (fluorescence excitation light and transmitted illumination light)
"<Arduino is ready>"
"-----------------------------------"
"Command List:"
"-----------------------------------"
"COMMAND: Fluorescence excitation"
"SYNTAX:  fe/color_on/off/"
"EXAMPLE: fe/blue_on/"
"-----------------------------------"
"COMMAND: Bright Field"
"SYNTAX:  bf/(r,g,b)/"
"EXAMPLE: bf/(000,064,254)/"
"-----------------------------------"
"COMMAND: Dark Field"
"SYNTAX:  df/(r,g,b)/"
"EXAMPLE: df/(000,064,254)/"
"-----------------------------------"
"COMMAND: Phase Contrast"
"SYNTAX:  pc/(r,g,b)/field_direction(bt/bb/bl/br/dt/db/dl/dr)"
"EXAMPLE: pc/(000,064,254)/bl/"
"-----------------------------------"
"COMMAND: Multiple Points"
"SYNTAX:  mp/number(1~255)/(r,g,b)num1/(r,g,b)num2/.../"
"EXAMPLE: mp/3/(000,064,254)254/.../"
"-----------------------------------"
"COMMAND: Set Parameters"
"SYNTAX:  set/dist/num/"
"         set/na/num/"
"EXAMPLE: set/dist/90/"
"         set/na/0.5/"
"-----------------------------------"
"COMMAND: Turn Off All"
"SYNTAX:  x/"
"-----------------------------------"
"-----------------------------------"
