#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Mar 17 22:59:45 2019

@author: 2020shatgiskessell
"""
import cv2
import numpy as np
print (np.__version__)
#requires numpy 1.1
template = "/Users/2020shatgiskessell/Desktop/Wheres_Waldo/template.png"
img = "/Users/2020shatgiskessell/Desktop/Wheres_Waldo/Test_Images/W3W2bjH.jpg"

from siftmatch import match_template
match_template(img, template, 5, 1)

