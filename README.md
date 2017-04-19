X3F Wrapper
-----------


This project offers the ability to quickly convert a directory filled with X3F images into jpgs, tiffs, or dng files, using the Kalpanika X3F Tools executable.  In addition, this tool will copy exif information from the source file into the final image.

To set it up:

1. Download the binary file.  (Only windows at this point, Mac and Linux coming soon).
2. Set up parameters (denoising, file output type, etc) in the preferences dialog, if you want something other than the defaults (denoised linear DNG).

As of version 0.56.2, both the x3f_extract.exe and exiftools.exe programs are installed in the release binary, and the default settings point to the distributed executables.  If you want different versions of those executables, you can select them in the preferences dialog.

To run it:
Either double click on individual files, or convert all unprocessed images via the button in the bottom right.

Please let us know if you have any issues with the conversion!

For Mac Developers:

To create a dmg for installation:

1. copy `x3f_extract_mac` into the `x3f_wrapper.app/Contents/MacOS` directory
2. copy the `opcodes` folder into the `x3f_wrapper.app/Contents/MacOS` directory
3. rename the `x3f_extract_mac` program in the `x3f_wrapper.app/Contents/MacOS` directory to `x3f_wrapper`
4. run the following command from a directory where `x3f_wrapper.app` is visible:

    <QTDIR>/bin/macdeployqt x3f_wrapper.app/ -dmg

And you should now have an installable dmg file.
