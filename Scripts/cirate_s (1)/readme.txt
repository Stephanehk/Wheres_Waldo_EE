Cirate_s.zip file contains improved pyramidal Ciratefi demonstration programs and image files. 

Ciratefi (Circular, Radial and Template-Matching Filter) is a template matching technique invariant to rotation, scale, translation, brightness and contrast. Ciratefi was defined in paper [Ci22]. 

The tests are to be executed in Windows XP, Vista or 7. 

The content of this ZIP file is:

readme.txt = this readme file

run-c.bat  = batch file to run color template matching
run-g.bat  = batch file to run grayscale template matching

ciratecs.cfg = improved pyramidal color ciratefi configuration file
ciratecs.cpp = source program
ciratecs.exe = executable program

cirategs.cfg = improved pyramidal grayscale ciratefi configuration file
cirategs.cpp = source program
cirategs.exe = executable program

Note: The source programs makes use of libraries Proeikon and OpenCV. They cannot be compiled alone.

cv110.dll      = OpenCV library
cxcore110.dll  = OpenCV library
highgui110.dll = OpenCv library

a1.jpg    = 3 color images to be analyed
a2.jpg
a3.jpg
q01.ppm   = 16 color query template images
q02.ppm
q03.ppm
q04.ppm
q05.ppm
q06.ppm
q07.ppm
q08.ppm
q09.ppm
q10.ppm
q11.ppm
q12.ppm
q13.ppm
q14.ppm
q15.ppm
q16.ppm

To run the tests, decompress the content of this ZIP file in a directory and run:
C:\directory>ciratecs ciratecs.cfg a1.jpg q01.ppm c101.ppm (runs one color template matching)
or 
C:\directory>cirategs cirategs.cfg a1.jpg q01.ppm g101.ppm (runs one grayscale template matching)
or
C:\directory>run-c (runs all color template matchings)
or 
C:\directory>run-g (runs all grayscale template matchings)

Many intermediary files will be generated that allows understanding the involved process.

[Ci22] H. Y. Kim and S. A. Araújo, "Grayscale Template-Matching Invariant to Rotation, Scale, Translation, Brightness and Contrast," IEEE Pacific-Rim Symposium on Image and Video Technology, Lecture Notes in Computer Science, vol. 4872, pp. 100-113, 2007.
