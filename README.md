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
3. For each camera, make small cropped images of each of the fiducial marks to be tracked by the camera. Try to choose marks such that the cropped images will be free of patterns that could distract the image matching algorithm. Ensure that the entire mark is captured in the new template, and that a small amount of padding is present around the mark. The choice of padding is a bit of an art. The padding can be used to lead opencv away from features that can sometimes have similar shading, such as due to lighting changes. As each template is made, record the X and Y position (in pixels) of the pixel in the top left corner of the cropped area. Do not partially crop pixels, as this will make it more difficult to asign the correct X and Y poisitions. Must have at least 4 templates.
4. Save the new templates in directories labelled `cam0`, `cam1`, etc, along with the renamed image from step 2.
5. Update the total number of templates per camera, the template file names, and pixel coordinates in the `LoadTemplatesCamX` functions in `LoadTemplates.cpp`.
6. Make and run Optometrist on the same run that the new templates were taken from.
7. Load the output file, and look at the `Key Points` for the event that the templates were taken from. This is where Optometrist has decided that each template was taken from in the image.
8. Ensure that the locations of the `Key Points` (rounded to the nearest pixel) matches what is written in the `LoadTemplates.cpp` file, and update the `LoadTemplates.cpp` file as needed and make again. Also check that the perspective transform is very close to a 3x3 identity matrix for the reference event.
9. Have a new optical lookup table generated using the same event as a reference.
