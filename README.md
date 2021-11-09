**Build instructions**
    
Pull from git:

```
git clone https://github.com/picoexperiment/PICOJarTracker.git
```

Ensure cmake and opencv are both installed. On Compute Canada, use:

```
module load cmake
module load opencv
```

To build:

```
cd PICOJarTracker
cmake .
make
```

**Updating Templates**

The templates are images of the fiducial marks that are cropped from a reference image. These templates are numbered, with the number used as a reference in the code. The code will align the templates with the corresponding fiducial marks in images it receives from various events.

To update the templates, follow the steps below.

1. Choose a reference event, where all cameras are operational.
2. Copy the first frame from each camera and change the file name so future users will know from which event the image was taken.
3. For each camera, make small cropped images of each of the fiducial marks to be tracked by the camera. Ensure that the entire mark is captured in the new template, and that a small amount of padding is present around the mark. As each template is made, record the X and Y position (in pixels) of the pixel in the top left corner of the cropped area.
4. Save the new templates in directories labelled `cam0`, `cam1`, etc, along with the renamed image from step 2.
5. Update the total number of templates per camera, which is hard-coded in `NumMarks.hpp`.
6. Update the template file names and pixel coordinates in the `LoadTemplatesCamX` functions in `LoadTemplates.cpp`.
7. Make and run Optometrist on the same run that the new templates were taken from.
8. Load the output file, and look at the `Key Points` for the event that the templates were taken from. This is where Optometrist has decided that each template was taken from in the image.
9. Ensure that the locations of the `Key Points` (rounded to the nearest pixel) matches what is written in the `LoadTemplates.cpp` file, and update the `LoadTemplates.cpp` file as needed and make again.
10. Have a new optical lookup table generated using the same event as a reference.
