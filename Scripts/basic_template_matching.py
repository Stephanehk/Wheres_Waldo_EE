#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Mar 15 17:48:38 2019

@author: 2020shatgiskessell
"""

import cv2
import numpy as np
import timeit
from matplotlib import pyplot as plt

template = cv2.imread("/Users/2020shatgiskessell/Desktop/Wheres_Waldo/template.png")
img = cv2.imread("/Users/2020shatgiskessell/Desktop/Wheres_Waldo/Test_Images/wheres-waldo.jpg")


def template_matching(roi, template):
    h,w,_ = template.shape
    #methods = ['cv2.TM_SQDIFF', 'cv2.TM_CCORR','cv2.TM_CCORR_NORMED']
    methods = ['cv2.TM_CCORR_NORMED']
    
    for method in methods:
        method = eval(method)

        # Apply template Matching
        res = cv2.matchTemplate(img,template,method)
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    
        # If the method is TM_SQDIFF or TM_SQDIFF_NORMED, take minimum
        if method in [cv2.TM_SQDIFF, cv2.TM_SQDIFF_NORMED]:
            top_left = min_loc
        else:
            top_left = max_loc
        bottom_right = (top_left[0] + w, top_left[1] + h)
    
        cv2.rectangle(img,top_left, bottom_right, 255, 2)
        cv2.imwrite("matched_" + str(method) + "10.png", img)
    
        plt.subplot(121),plt.imshow(res,cmap = 'gray')
        plt.title('Matching Result'), plt.xticks([]), plt.yticks([])
        plt.subplot(122),plt.imshow(img,cmap = 'gray')
        plt.title('Detected Point'), plt.xticks([]), plt.yticks([])
        plt.suptitle(str(method))
        plt.show()

start = timeit.default_timer()
template_matching(img, template)
stop = timeit.default_timer()
run_time = stop - start
print('Time: ', str(run_time))  