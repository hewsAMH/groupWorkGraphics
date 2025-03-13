# Computer Graphics - Scene Graph Implementation

## Features Implementation Status

### a. Working Features - all features work as intended in the assignment discription. 
- **Part 1**: Drone succesfully modelled
  - Fuselage is created and a nose indicates the front of the drone
  - Four propeller fans are included, one on each corner of the fuselage
  - There are four "feet" upon which the drone can land

- **Part 2**: Drone succesfully animated
  - The drone is perpetually flying; its propellers are always rotating
  - The speed of the propellers, and the drone by extension, can be slowed using the 's' key and sped up using 'f' key
  - Pressing 'j' causes the drone to do a full sideways roll

- **Part 3**: Keyboard control succesfully implemented
  - Using the '+' and '-' keys, the drone moves forward and backward relative to where it's facing.
  - The arrow keys cause the drone to turn left, right, up, or down in place
  - The 'd' and 'D' keys reset the drone's position

- **Part 4**: Multiple cameras succesfully implemented
  - The global camera (toggled with '1') allows for easily viewing the entire scene from a third person point of view
  - The chopper camera (toggled with '2') circles the scene overhead, completely independent of the drone's movements
  - The 1st-person camera (toggled with '3') is placed at the nose of the drone and shows movements as the drone sees them

### b. Non-working Features
- None. All required features have been successfully implemented.

## Team Contribution Breakdown

- **Abigail Hews**:
  - Part 1:
    - Base of the drone model
    - Fully functional propellers
    - Base of the drone's feet
  - Part 2:
    - Implemented propeller animation
    - Implemented most of the roll
    - Implemented slower/faster key input
  - Part 3:
    - Implemented all keypresses for turning
    - Added keypresses for forward/backward
    - Finished the reset function
  - Part 4:
    - Fully created the global camera
    - Largely created the chopper camera
    - Created the start of the first-person camera

- **Aidan Johansson**:
  - Part 1:
    - Finishing the base drone model
    - Adding a nose to distinguish the front
    - Polishing up the drones' feet
  - Part 2:
    - Completed the roll
  - Part 3:
    - Implemented the logic for correct turning from all camera angles
    - Implemented the logic for forward/backward movement relative to the drone
  - Part 4:
    - Removed the connection between the chopper camera and the drone
    - Implemented correct logic for the first-person camera

## Additional Notes for Graders

- None, all parts work as expected. 

## File Structure
- `DroneAnimation.cpp`: File containing the main function for this program, results in the file `DroneAnimation`
- All new functionality is implemented within Model, View, and Controller