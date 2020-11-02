%supplementary material for paper: 
%Low-cost illumination module for multi-channel fluorescence microscopy

%image data acquisition workflow:

%Open the matlab data acquisition toolbox to preview.
%Focus the sample by two cameras using green fluorescence channel and green transmission channel,then close preview.
%check the serial port number of Arduino board;
%check the scanning step size of the Piezo motor;
%change the exposure time for both illumination systems based on preview;
%change the saving file folders’ names;
%check or change the illumination mode based on your requirements.
%I'd change to 10x and move manually to the bottom right of the sample
%I will go down(z) so out-focus little bit for initial state
%Run the full script , which basically scans the whole sample! 


%% Initialize ActiveX Controller for Thorlabs Piezo Motor.
clear; close all; clc;
tic

global h; 
% make h a global variable so it can be used outside the main
% function. Useful when you do event handling and sequential move

% Create Matlab Figure Container for Thorlabs Piezo Motor
fpos    = get(0,'DefaultFigurePosition'); % figure default position
fpos(3) = 650; % figure window size;Width
fpos(4) = 450; % Height

f = figure('Position', fpos, 'Menu','None','Name','APT GUI');

% Create ActiveX Controller
h = actxcontrol('APTPZMOTOR.APTPZMotorCtrl.1',[20 20 600 400 ], f);

% Set the Serial Number
SN = 97100284; % put in the serial number of the motor hardware
set(h,'HWSerialNum', SN);

% Start Control
h.StartCtrl;

% parameters no need to change
channel_x = 0;  % x direction motor index
channel_y = 1;  % y direction motor index
channel_z = 2;  % z direction motor index
voltage = 110;
max_velocity = 2000; %cycles/sec ~ 40um/sec
acceleration = 100000;  %cycles/sec/sec ~ 2000um/sec/sec

h.SetDriveOPParams(channel_x, voltage, max_velocity, acceleration);  %initialize x direction motor
h.SetDriveOPParams(channel_y, voltage, max_velocity, acceleration);  %initialize y direction motor
h.SetDriveOPParams(channel_z, voltage, max_velocity, acceleration);  %initialize z direction motor

%% Initialize BASLER camera 1 for fluorescent image acquisition.
imaqreset
vid_fluorescent = videoinput('gentl', 1, 'Mono12');
src_fluorescent = getselectedsource(vid_fluorescent);
vid_fluorescent.FramesPerTrigger = 1;

%% Initialize BASLER camera 2 for transmitted light illumination image acquisition.
vid_transmission = videoinput('gentl', 2, 'Mono12');
src_transmission = getselectedsource(vid_transmission);
vid_transmission.FramesPerTrigger = 1;

%% Initialize the LED array disk controlled by arduino.
delete(instrfindall);
s = serial('COM6');
set(s,'BaudRate',2000000);
set(s,'Timeout',30);
set(s,'InputBufferSize',8388608);

fopen(s);
if (exist('board1','var'))
    board1.stop;pause(0);
end

%% Set the Scanning Parameters.
FileN_transmission = 'C:\yihui\images\fluorescence_data_acquisition\transmission_illumination\' ;
% set the directory for saving the collected transmitted light illumination images

FileN_fluorescent = 'C:\yihui\images\fluorescence_data_acquisition\fluorescent\' ;
% set the directory for saving the collected fluorescent images

movetimes_x = 3; % numbers of image scanning steps in x direction
movetimes_y = 3; % numbers of image scanning steps in y direction
movetimes_z = 2; % numbers of image scanning steps in z direction

movedistance_x = 60000;  % total distance of image scanning acquisition in x direction ~ 1200um
movedistance_y = 50000;  % total distance of image scanning acquisition in y direction ~ 1000um
movedistance_z = 3000;   % total distance of image scanning acquisition in z direction ~ 60um

% set the exposure time for each radius and each color of transmitted light illumination 
% you can add more modes based on your requirements
exposure = [400000 300000 300000;...   
    500000 300000 300000;...
    500000 300000 300000;...
    500000 400000 300000;...
    1000000 700000 500000;...
    1000000 700000 500000;...
    1000000 700000 500000;...
    1000000 700000 500000;...
    1000000 700000 500000;...
    1000000 700000 500000];

% set the exposure time for each color of fluorescent excitation light illumination 
% you can add more modes based on your requirements
exposure_fluorescence = [800000];

%start index of the disk LED array for each illumination radius
start_points = [254 253 247 235 215 191 163 131 91 47];
%end index of the disk LED array for each illumination radius
end_points = [254 248 236 216 192 164 132 92 48 0];

% I'd change to 10x and move manually to the bottom right of the sample
% I will go down(z) so out-focus little bit for initial state

% Set image acquisition move position matrix
a = [1:movetimes_x movetimes_x:-1:1];
a_ = cat(2, repmat(a, 1, fix(movetimes_y / 2)), a(1:rem(movetimes_x * movetimes_y, length(a))));
a_ = repelem(a_, movetimes_z);
b_ = repelem([movetimes_y:-1:1], movetimes_x * movetimes_z);
c = [1:movetimes_z movetimes_z:-1:1];
c_ = cat(2, repmat(c, 1, fix(movetimes_x * movetimes_y / 2)), c(1:rem(movetimes_x * movetimes_y * movetimes_z, length(c))));
movestate_mat = [a_; b_; c_]';
movestate_mat2 = [movestate_mat(2:length(movestate_mat), :); [0, 0, 0]];
movestate_mat_final = movestate_mat2 - movestate_mat;
movestate_mat_final = [[0, 0, 0]; movestate_mat_final(1:length(movestate_mat_final) - 1, :)];

%% Sending Moving Commands and take images all together.
for i = 1:length(movestate_mat_final)
    %{
    if mod((i - 1), movetimes_z) == 0
        disp("Adjust focusing level if it needs. Press Enter after focusing..")
        pause;
    end
    %}
    position = sprintf('x%02i_y%02i_z%02i',movestate_mat(i, 1),movestate_mat(i, 2), movestate_mat(i, 3));
    disp(position);
    % display current position
    movechannel = find(movestate_mat_final(i, :)); %get the channel number
    if isempty(movechannel)
        movechannel = 0;
    end
    
    switch movechannel
        case 0
            pause(1);
        case 1
            h.MoveRelativeStepsEx(channel_x, movestate_mat_final(i, movechannel) * movedistance_x, 1); % move the stage in x direction 
            pause(1);
        case 2
            h.MoveRelativeStepsEx(channel_y, movestate_mat_final(i, movechannel) * movedistance_y, 1); % move the stage in y direction 
            pause(1);
        case 3
            h.MoveRelativeStepsEx(channel_z, movestate_mat_final(i, movechannel) * movedistance_z, 1); % move the stage in z direction 
            pause(1);
        otherwise
            error('movestate_mat_final has an error')
    end
    
    % Control the LED fluorescence illumination system
    fprintf(s,'x/');% turn off all the disk array LEDs
    
    color = ["blue"];% set the LED color, you can add more colors based on your requirements
    for k = 1:length(color)
        command = sprintf('fe/%s_on/',color(k)); % turn on the excitation LED in each color
        fprintf(s,command);
        
        src_fluorescent.ExposureTime = exposure_fluorescence(k); % set the exposure time for fluorescent image
        src_fluorescent.Gain = 0;
        img = getsnapshot(vid_fluorescent); % acquisite the fluorescent image
        pause(0.2);
        fname_fluorescence = sprintf('%sx%02i_y%02i_z%02i_%s_excitation%s', FileN_fluorescent, movestate_mat(i, 1),movestate_mat(i, 2), movestate_mat(i, 3),color(k),'.tif');
        imwrite(img, fname_fluorescence);
        % save the fluorescent image to the directory
        
        command = sprintf('fe/%s_off/',color(k)); % turn off the excitation LED
        fprintf(s,command);
    end
    
    % Control the LED array
    % mode: individual LED illuminations for all three colors and all radius
    % you can change the illumination mode based on your requirements
    disp('Turning on LED    ');
    % Turn on the three color LEDs at ten radius in turn for illumination
    for color = 1:3
        if color ==1
            r = 255; g = 0; b = 0;
        elseif color ==2
            r = 0; g = 255; b = 0;
        elseif color ==3
            r = 0; g = 0; b = 255;
        end
        
        for ring = 1:10
            src_transmission.ExposureTime = exposure(ring,color); % set the exposure time for single point illumination image
            src_transmission.Gain = 0;
            for index = start_points(ring):-1:end_points(ring)
                command = sprintf('mp/1/(%03i,%03i,%03i)%03i/', r,g,b,index); % turn on the single point LED in each color and radius
                fprintf(s,command);
                %s.ValuesSent
                spot = sprintf('\b\b\b\b%03i',index);
                disp(spot); %display the LEDs index 
                
                % take and save a snapshot using the camera
                imx = getsnapshot(vid_transmission); % acquisite the single point illumination image
                pause(0.2);
                fname_tif = sprintf('%sx%02i_y%02i_z%02i_index%03i_ring%02i_r%03i_g%03i_b%03i%s', FileN_transmission, movestate_mat(i, 1),movestate_mat(i, 2), movestate_mat(i, 3),index,ring,r,g,b,'.tif');
                imwrite(imx, fname_tif);
                % save the single point illumination image to the directory
            end
        end
    end
    
end

%% Disconnect to the LED array.
fprintf(s,'x/');
%close the Serial port to the LED array
fclose(s);

%% reset all image acquisition connections.
imaqreset;
toc