# Darth Faders Puzzle - Build/Install Guide
The Darth Faders Puzzle Trigger online guide explains in great detail the hardware, software and wiring for this project. We encourage you to read this first to get comfortable and familiar with the project and it's components.

## 3D Models
There are 7 pieces to the project
1. (1) Base
2. (1) Body
3. (1) Cap
4. (1) Crown
5. (3) Button Frames 

##Build
1. Install necessary library a board objects to Arduino IDE, and upload code to your RP2040.
2. Separate and pull apart all your jumper pins. (For example, you’ll need 4 each of Yellow, Red and Green, 4 Orange, 10 Black, 3 Brown, 1 Blue, 1 Purple, 1 White)
3. Remove the plastic housing from one side of each wire (use the tip of an exacto blade to lift up the small center tab and the casing slides right off). Then using your wire snips to snip off the aluminum pin just past the widest part closest to the wire. This will leave a short metal stub that will fit PERFECTLY inside the terminal holes. It create a tight and solid fit while keeping the integrity of the wire in tack and leave you with a sharp pin on the other to solder in all the through holes of the PCB’s and pins.
4. Solder your BUCK Converter to your Breakout board. Look at the bottom of your breakout to find the ground rail that runs across an entire row.f you put your 2 pin terminal block on that end and you place your buck just below that row, it’s easy to connect your OUT- and IN- to the GRN row and then up to one of your terminal pins. All that’s left is connecting OUT+ to USB hole, and IN+ up to the other pin on your terminal block.
5. Use volt meter to double check voltages. Should have 5v coming out of OUT+. OUT+ should be connected to USB. THIS IS CRITICAL. We do not want to connect the stack and later try to power the RP2040 with the 9v Power supply if this converter isn't properly connected!
6. Solder up your wires to your Sliders. I used colored wires on the center wiper pins so I could ensure I knew which slider goes to which color. Used Red for left pin (3.3v) and black or brown for GND pin up on the top side near the motor.
7. For each motor also use Black wire for one side and either a Red, Green or Yellow wire for the other pin. You will now have two Red, Yellow or Green wires coming off each slider/pot. One for the motor feather, and the other for A0, A1, and A2 pins on the breakout. 
8. Use small shrink tubes to keep things tidy. I used 3 (Top middle and bottom) to connect/group the 3 slider Grounds, 3 to group the 3.3v power lines, and 3 to connect each set of motor wires. This creates a sort of ‘slider chassy’ to keep your wires under control
9. Solder your Step Switches to their PCB Boards.  You’ll have 4 wires coming off each step switch PCB (A black LED-, a Brown Common and either RED, GREEN or YELLOW wires for both the LED+ and N-OPEN pins (mark the end of LED+ with black marker or piece of tape. I group the Commons together and the LED- wires Together. (Again using 3 small shrink tubs one near the pin connections to the PCB, one in the middle, then one near the other end (making sure not to get to close to the end as they need to separate) and do the same for the two colored wires ( LED+ and N-OPEN) from each button/PCB. (This is why we mark/identify the LED+ wire) 
10. Solder wires to your I2C backpack. Red Power, Black GND and Yellow and Orange for Clock and Data (or any other colors for that matter)
11. Now it’s time to wire it all up. Take extra care to get wires to the correct pins. They don't mark the breakout board with the RP2040 numbers so It takes effort to find the right one.
12. Use wire terminals to get your Ground wires down to three wires 
    1. Ground #1 (common lines from step switches
    2. Ground #2 (LED- lines from step switches, the GND line from momentary button and GND line if for your trigger line (if you need to access the trigger on success)
    3. Ground #3 (GND Lines from Sliders)
13. Use a wire terminal to group your 3.3v power lines from your sliders together 
    1. 3.3v #1 (power lines from the 3 sliders)
    2. 3.3v #2 (power line from I2C Backpack
14. Wire to breakout based on documentation leaving the 6 motor wires last (2 for each motor)
15. Plug RP2040 in to breakout.
16. Plug Motor Breakout in to RP2040.
17. Wire the motors to M1, M2 and M3 (Referring wiring diagram for proper terminals.
18. Screw the Motor ends of the sliders to the base of the model using M3x6 screws. Green left, Yellow Middle, Red right and set breakout on pins to hold in place.
19. Have Arduino IDE open with Serial Monitor set to 15500)
20. Ensure you have not yet connected your two 2 pin power terminals together (Motor breakout and BUCK). This will be the very last thing after all troubleshooting 
21. plug in 9v barrel 
22. Plug in USB-C.
23. Watch your Serial Monitor. Troubleshoot as needed (see troubleshooting section)
24. Once everything is working, unplug all power. Run wire from Motor Breakout + to + terminal connected to Buck. Do same for GND.
25. You should now be able to plug the 9v Barrel in and everything should run (you will not have access to serial monitor now). Run voltage tests on breakout (i.e. USB pin) with stack removed before connecting stack and powering by 9v to ensure 5v is being delivered to USB pin when 9v power is supplied to Motor breakout.
26. Finish assembly of model. Place BODY down in position of BASE
27. Pull the three button assemblies up though the top and screen in to place using M3x6 screws
28. Insert buttons in to openings of CROWN keeping color the same orientation as the sliders.
29. Use the Button frames to keep buttons in place using super glue as needed (TAKE EXTREME CARE to not get glue on button hinge, terminals or springs.
30. Use super glue to carefully glue CROWN to CAP TAKING EXTREME CARE not to use too much glue so that it seeps out edges. A few dots on each corner should be sufficient.
31. Glue Numeral Display to Backpack TAKING EXTREME CARE not to exceed past or overlap in the path of the Yellow slider path. 
32. Ta Da! Your done. Take pride in your Darth Faders Puzzle Trigger project.

## Lessons Learned
LESSONS LEARNED:

First, I was really struggling with the pins on the motorized sliders. Not sure why because looking at the data-sheet it’s very clear. I just saw other things that contradicted it and made it more complicated than it needed to be and they really don't articulate what pin is what in the Darth Faders project. Wont bore you with all the super gory many hours of details, but finally sorted it out. If you’re looking at the back of the slider with the motor at the top. The GROUND pin is on the top left side next to the motor (Strange place for it, but is what it is). The POWER pin is on the bottom left, the WIPER pin is in the middle, and the touch pin is on the bottom right (Not using this pin in my project). Do not swap the GND and POWER pins. If you do, the auto calibration routine on start up wont work right and you’ll get some cooky LED action happening. (PART OF MY STRUGGLE)

The breakout board has a 3 GND ports. We should use them all. Use a different GND port for each group of grounds. IN other words, put the 3 grounds for the step switch LED wires (along with the GND wires from the button and the Backpack) on one GND port, the 3 Common wires from the step switches on a separate GND port, and the ground wires from the sliders on a 3rd GND port. Don’t group all your ground wires together. You’ll get some icky LED wiper readings and Backpack LED behavior when you do that (PART OF MY STRUGGLE). 

This one is obvious but it’s worth a gentle reminder: Make sure you check all your solder points on your PB85 breakout boards for your switches, and make sure you use the Normally Open pin(NO-O).  The Normally Closed (NO-C) pin should be empty. (PART OF MY STRUGGLE) 

You must solder the A0 pins together on the backpack to change the address. If you don't, you’ll get a conflict with the motor breakout. The pins on the backpack go on the top when mounting in its opening on the model. (If you put it in upside down, the counter will go top to bottom instead of bottom to top. ) it’s designed so #1 is at the bottom and #24 is at the top. 

If you dig in to the code, you’ll may see there are some functions on the Motor.cpp file that aren’t used(tried cleaning them up). I tried to do some fancy slider dancing on a successful puzzle solve, but couldn’t get the sliders to get to the middle of the track correctly. I’m sure there was a better way of doing it, but just didn’t happen. As such the sliders do a quickly little UP->DOWN-> dance on success. 

IF you need to troubleshoot, MAKE SURE you don't have the BUCK converter powering your RP2040. You’ll need the 9v barrel power to power the motors and the USB-C plugged in to your RP2040 to see the serial port data. If you plug both of those in AND have the BUCK converter taking power from the 9V power and feeding the RP2040, you’ll make smoke and be sad.

I soldered the buck converter to the available breakout pins on the breakout board. It’s really nice they developed it so you can put additional components on the Breakout board when you need. (We need). I then used breadboard jumper wires to neatly connect things together. My soldering job was just pitiful but worked none the less. Soldering the BUCK to the Breakout makes things nice and tidy because we are going to have a *#&$^% ton of wires when this is all done. We don't need yet another object floating in space.

I put a 2 pin port block on the end of the breakout to give me easy access to IN power and ground on the BUCK
Soldered jumpers from one pin to IN+ and one pin to IN-
OUT- and IN- I ran to the GND rail that runs across the bottom (look at the bottom of the breakout board labeling to find it)
The OUT+ goes to USB.
I had to undo it once because I soldered things backwards. The BUCK is labeled on the bottom so when looking at another for reference, my brain flipped things and I soldered things wrong. (PART OF MY STRUGGLE) Now I just have to run to wires from the 2 pin Port Block on the Motor Breakout to the 2 pin Port block on the Bottom Breakout board. 
This means two wires are going in to the power Ports on the Motor Breakout. Coming IN from the Barrel port, the OUT to the BUCK.
I did all this so we could just use the 9V barrel power instead of needing both cables.

Make sure you use long enough wires to connect things together. You’ll board the struggle bus if things are too short or exactly the right length. Make things longer then user good wire management to clean things up (zip ties and heat shrink tubes)

##Troubleshooting

If your Backpack LED's are only lighting up a short distance:
 (i.e. only 6 high for example) you likely have your wiring wrong on your sliders. Check your ground connections, make sure the left wire is wired to 3.3v and GND to GND, and center pin is your wiper pin connect to the correct pin corresponding to that correct color. Also make sure on boot up that the High = Highst value, and Min = Lowest value. If these are backwards, things wont work.

If your LED Backpack matrix isn't working:
Make sure you have connected/soldered the A0 pins together on the back of the Backpack to change it's address. Out of the box, both it and the motor feather have the same I2C address.
Make sure you have Data to Data and Clock to Clock 

If your motors arn't working:
Make sure your motor feather has power (should have a green LED)
Make sure it's security stacked on top of your RP2040 (no pins are bent or missed their connection hole)
You have them connected to M1, M2 and M3 ports (GND wires should be in the middle connectors and the color wires (Red, Yellow and Green) should be on the outer ends.

If things don't work after connecting BUCK to breakout:
Remove the stack from the breakout.
You should have done a voltage test on the BUCK before you connected anything to it ensuring the OUT+ is sending 5v and that it's connected to USB.
Make sure you dont have solder gaps or bleeds.
Wire the 9v Barrel port directly to the breakout with the stack removed. Plug in the power and run your voltage tests again. IN+ should be 9v, OUT+ should be 5v.
Test the USB by connecting your probes to GND and the USB screw on the breakout. Should be 5v
Test to make sure you have connections between OUT+ and USB, as well as OUT- to ground.


## Quick Operation Reference

SET THE CODE
1. No step switches pressed + hold momentary 5s -> SET mode (all LEDs blink fast)
2. Press a step switch -> blinks slow -> move slider to select code -> bargraph fills that color
3. Press same switch again -> locked (LED solid, single bargraph LED lit)
4. Repeat for remaining two colors
5. Press momentary once -> saved -> celebration sequence -> IDLE

SOLVE THE PUZZLE
1. Press any step switch -> blinks slow (solve mode)
2. Move slider -> bargraph fills in that color
3. Press same switch -> locked (LED solid)
4. Repeat for other two colors (any order)
   CORRECT -> celebration + 3s HIGH on GPIO37
   WRONG   -> all off + sliders reset to bottom -> IDLE

REVEAL STORED CODE
1. Press each step step switch once -> all blink slow
2. Hold momentary 5s -> sliders move to stored positions, bargraph shows values
3. Press momentary once -> sliders return to bottom -> IDLE

FACTORY RESET
Hold momentary 10s (no switches in solve mode) -> code resets to 7/7/7 -> IDLE
