Cirateg.zip file contains Ciratefi demonstration programs and image files. Ciratefi (Circular, Radial and Template-Matching Filter) is a template matching technique invariant to rotation, scale, translation, brightness and contrast. Ciratefi was defined in paper [Ci22]. The tests are to be executed in Windows XP (though probably they will run correctly in other MS-Windows versions). The content of this ZIP file is:

a.jpg         = The image to be analyzed
cirateg.exe   = Ciratefi program for grayscale images
cirateg.png   = A chart depicting the data flow in Ciratefi
first.cfg     = Configuration file to be used only in the first program execution
others.cfg    = Configuration file to be used in the second and subsequent executions
q-dog.jpg     = Dog query image
qbear.jpg     = Bear query image
qesse.jpg     = "S" query image
qfrog.jpg     = Frog query image
qpalm.jpg     = Palm tree query image
readme.txt    = This readme file
runtest.bat   = BAT file that runs the tests

To run the tests, decompress the content of this ZIP file in a directory and run:
C:\directory>runtest

Many intermediary files will be generated. The final output files are:
pbear.jpg, p-dog.jpg, pesse.jpg, ppalm.jpg, pfrog.jpg = Detection of 8 best matchings separated by at least 20 pixels.

[Ci22] H. Y. Kim and S. A. Araújo, "Grayscale Template-Matching Invariant to Rotation, Scale, Translation, Brightness and Contrast," IEEE Pacific-Rim Symposium on Image and Video Technology, Lecture Notes in Computer Science, vol. 4872, pp. 100-113, 2007.
