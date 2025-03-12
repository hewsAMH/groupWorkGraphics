# Computer Graphics - Scene Graph Implementation

## Features Implementation Status

### a. Working Features - all features work as intended in the assignment discription. 
- **Part 1**: Scene graph text rendering visitor successfully implemented
  - Correctly displays the hierarchical structure with tabs and hyphens
  - Properly shows node names as specified in the requirements

- **Part 2**: Scene creation via commands language
  - Rectangular ground base implemented
  - Building structures with turret and appropriate roof
  - At least 10 instances used in the building design
  - Modified humanoid scene graphs with various poses (at least 2 different posed humanoids)
  - Adjacent objects have distinct colors for visual clarity
  - Meaningful physical structure that could theoretically be 3D printed

- **Part 3**: Trackball implementation
  - Full 3D rotation of the scene using mouse drag
  - Vertical mouse movement rotates around X axis
  - Horizontal mouse movement rotates around Y axis
  - Diagonal movements produce appropriate combined rotations
  - 'R' key resets the trackball to initial view
  - No gimbal lock issues
  - Rotation speed balanced for intuitive control

### b. Non-working Features
- None. All required features have been successfully implemented.

## Team Contribution Breakdown

- **Abigail Hews**:
  - Complete implementation of Part 1 (Scene graph text rendering visitor)
  - Part 2 implementation, including:
    - Scene graph command structure
    - Ground base implementation
    - Building design and implementation
    - Material and color management

- **Aidan Johansson**:
  - Part 2:
    - Humanoid pose modifications
    - Scene integration and refinement
  - Complete implementation of Part 3 (Trackball rotation mechanism)

## Additional Notes for Graders

- None, all parts work as expected. 

## File Structure
- `courtyard-scene-commands.txt`: Main scene file that creates all building components
- `sitting-humanoid-commands.txt`: First posed humanoid definition
- `looking-humanoid-commands.txt`: Second posed humanoid definition
- `TextScenegraphRenderer.h`: a new operation on scene graphs that produces such a text rendering
